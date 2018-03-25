#include "blockpad.h"
#include "ui_BlockPad.h"
#include "global.h"
#include <QStringList>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include "crypt.h"
#include <QMessageAuthenticationCode>
#include "codeeditor.h"
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "adswebpage.h"
#include <QtConcurrent>
#include <QNetworkReply>
#include <QProgressDialog>
#include <QNetworkAccessManager>
#include <QCoreApplication>
#include <QProcess>
#include <QDesktopWidget>

BlockPad::BlockPad(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BlockPad)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentWidget(ui->Development);
    //load settings
    {
        if(settings.value("MakePasswordsHidden").type() != QVariant::Invalid)
        {
            ui->tableWidgetAccounts->slotAllwaysChecked(
                        settings.value("MakePasswordsHidden").toBool());
        }
    }
    //signals-slots connects
    {
        connect(ui->pushButtonCompleteRow, &QPushButton::clicked,
                this, &BlockPad::slotCompleteRowClicked);
        connect(ui->pushButtonUpdate, &QPushButton::clicked,
                this, &BlockPad::slotUpdateClicking);
        connect(ui->pushButtonSettings, &QPushButton::clicked,
                this, &BlockPad::slotSettingsClicked);
        connect(ui->pushButtonGeneratePassword, &QPushButton::clicked,
                this, &BlockPad::slotPasswGenClicked);
        connect(ui->pushButtonSave, &QPushButton::clicked,
                this, &BlockPad::slotSaveEncrypt);
        connect(ui->tableWidgetAccounts, &TableWidgetAccounts::sigCompetingRow,
                this, &BlockPad::slotRowSuccessfullyCompleted);
        connect(ui->tableWidgetCoinRecords, &TableWidgetCoinRecords::sigCompetingRow,
                this, &BlockPad::slotRowSuccessfullyCompleted);
        connect(ui->tabWidget, &QTabWidget::currentChanged,
                this, &BlockPad::slotCurrentWgtChanged);
        connect(ui->codeEdit, &CodeEditor::newChanges,
                this, &BlockPad::slotBlockPadNewChanges);
        connect(this, &BlockPad::sigUpdateAvailable,
                this, &BlockPad::slotUpdateAvailable);
        connect(this, &BlockPad::sig_No_UpdateAvailable,
                this, &BlockPad::slot_No_UpdateAvailable);
        connect(this, &BlockPad::sigErrorParsing,
                this, &BlockPad::slotErrorUpdateParsing);
    }
    slotCurrentWgtChanged();
    highlighter = new Highlighter(ui->codeEdit->document());
    ui->pushButtonSave->setEnabled(false);
    //add ads
    {
        ui->mainVerticalLayout->setAlignment(Qt::AlignCenter);
        QWebEngineView * webEngineView = new QWebEngineView(this);
        AdsWebEnginePage * page = new AdsWebEnginePage(this);
        webEngineView->setPage(page);
        ui->mainVerticalLayout->addWidget(webEngineView);
        webEngineView->setUrl(QUrl("http://blockpad.io/adserv1.php"));
        webEngineView->setFixedHeight(110);
    }
}

void BlockPad::slotUpdateClicking()
{
    QtConcurrent::run(this, &BlockPad::checkUpdates, true);
    ui->pushButtonUpdate->setEnabled(false);
}

void BlockPad::slot_No_UpdateAvailable()
{
    QMessageBox::information(nullptr, "BlockPad update","No updates available");
    ui->pushButtonUpdate->setEnabled(true);
}

void BlockPad::slotDownloadUpdateFinished(QNetworkReply *reply)
{
    auto error = reply->error();
    if(error != QNetworkReply::NoError)
    {
        auto data = QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(error);
        QMessageBox::critical(nullptr, windowTitle(), data);
        return;
    }
    //redirect
    {
        QVariant redirectUrl =
                 reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

        if(!redirectUrl.toUrl().isEmpty())
        {
            auto reply = namUpdate->get(QNetworkRequest(redirectUrl.toUrl()));
            connect(reply, &QNetworkReply::downloadProgress,
                    this, &BlockPad::slotDownloadUpdateProgress);
            return;
        }
    }
    auto dataReply = reply->readAll();

    QFile file(Utilities::appFilesDirectory() + "/SetupBlockPad.exe");
    file.open(QIODevice::WriteOnly);
    file.write(dataReply);
    file.close();

    if(QMessageBox::Yes == QMessageBox::question(nullptr, "BlockPad update",
    "BlockPad is opened\r\nUpdater will close it in order to process installiation\r\nContinue?"))
    {
        QProcess procFinishUpdate;
        procFinishUpdate.startDetached(Utilities::appFilesDirectory()+ "/SetupBlockPad.exe");
        exit(0);
    }
    ui->pushButtonUpdate->setEnabled(true);
}

void BlockPad::slotDownloadUpdateProgress(qint64 bytesReceived,
                                    qint64 bytesTotal)
{
    if(bytesTotal != 0)
        progressUpdates->setValue((bytesReceived*100)/bytesTotal);
}

void BlockPad::slotUpdateAvailable(QString link, QString version, bool bManually)
{
    QMessageBox mesBox;
    mesBox.setText("An update package is available, do you want to download it?");
    mesBox.setWindowTitle("BlockPad update");
    QPushButton * yesButton = mesBox.addButton("Yes", QMessageBox::YesRole);
    QPushButton * noButton = mesBox.addButton("No", QMessageBox::NoRole);
    QPushButton * noRemindButton;
    if(!bManually)
        noRemindButton = mesBox.addButton("No remind", QMessageBox::NoRole);
    mesBox.setIcon(QMessageBox::Question);
    mesBox.exec();
    if (mesBox.clickedButton() == yesButton)
    {
        downloadUpdateVersion(link, version);
    }
    else
    {
        if(!bManually)
        {
            if (mesBox.clickedButton() == noRemindButton)
            {
                settings.setValue("noUpdating", true);
            }
        }
        ui->pushButtonUpdate->setEnabled(true);
    }
}

void BlockPad::downloadUpdateVersion( QString link, QString version)
{
    progressUpdates = new QProgressDialog(this);
    progressUpdates->setAttribute(Qt::WA_DeleteOnClose);
    progressUpdates->setLabelText("Downloading BlockPad " + version + ":");
    progressUpdates->setWindowTitle("BlockPad update");
    progressUpdates->setGeometry(
                QStyle::alignedRect(
                    Qt::LeftToRight,
                    Qt::AlignCenter,
                    QSize(500,90),
                    qApp->desktop()->availableGeometry()
                )
            );
    progressUpdates->show();
    connect(progressUpdates, &QProgressDialog::canceled,
            progressUpdates, &QProgressDialog::close);
    connect(progressUpdates, &QProgressDialog::canceled,
            this, [this](){ui->pushButtonUpdate->setEnabled(true);});
    namUpdate = new QNetworkAccessManager(progressUpdates);
    connect(namUpdate, &QNetworkAccessManager::finished,
            this, &BlockPad::slotDownloadUpdateFinished);
    updatingReply = namUpdate->get(QNetworkRequest(QUrl(link)));
    connect(updatingReply, &QNetworkReply::downloadProgress,
            this, &BlockPad::slotDownloadUpdateProgress);
}

void BlockPad::checkUpdates(bool bManually)
{
    QNetworkAccessManager nam;
    QUrl url;
#if defined(WIN32) || defined(WIN64)
    url = QUrl("https://bintray.com/package/info/bloc10fintech/BlockPad/BlockPad_stable_windows");
#endif
    QNetworkRequest request(url);
    QEventLoop loop;
    connect(&nam, SIGNAL(finished(QNetworkReply *)), &loop, SLOT(quit()));
    auto reply = nam.get(request);
    if(!reply->isFinished())
         loop.exec();
    auto data = (QString)(reply->readAll());
    bool bSuccessParsing = false;
    QString latestVersion;
    QString downloadLink;
    //parsing data
    {
        auto indexStartDownloads = data.indexOf("<div id=\"downloads\" class=\"section\">");
        if(indexStartDownloads != -1)
        {
            auto indexVer = data.indexOf("<span class=\"small\">(ver:",
                                         indexStartDownloads);
            if(indexVer != -1)
            {
                auto indexEndLine = data.indexOf("\n", indexVer);
                latestVersion = data.mid(indexVer, indexEndLine-indexVer)
                        .remove("<span class=\"small\">(ver:")
                        .remove(" ")
                        .remove(")</span>")
                        .remove("\r");
                auto indexGlyphButton = data.indexOf("<div class=\"thumb glyph-button\">");
                if(indexGlyphButton != -1)
                {
                    auto indexHref = data.indexOf("<a href=");
                    auto indexEndHref = data.indexOf("\">", indexHref);
                    if (indexHref != -1 && indexEndHref != -1)
                    {
                        downloadLink = "https://bintray.com" +
                                data.mid(indexHref, indexEndHref-indexHref)
                                .remove("<a href=\"");
                        bSuccessParsing = true;
                    }
                }
            }
        }
    }
    if(!data.contains("No direct downloads selected for this package"))
    {
        if(!bSuccessParsing)
            emit sigErrorParsing();
        else
        {
            if(latestVersion != defVersionDB)
                emit sigUpdateAvailable(downloadLink, latestVersion, bManually);
            else
            {
                if (bManually)
                    emit sig_No_UpdateAvailable();
                else
                    ui->pushButtonUpdate->setEnabled(true);
            }
        }
    }
    else
    {
        if (bManually)
            emit sig_No_UpdateAvailable();
        else
            ui->pushButtonUpdate->setEnabled(true);
    }
}

void BlockPad::slotErrorUpdateParsing()
{
    QString mess;
#if defined(WIN32) || defined(WIN64)
    mess = "Error of check new updates. Please try new version in <a href=\"https://bintray.com/bloc10fintech/BlockPad/BlockPad_stable_windows\">BlockPad Windows Version</a>";
#endif
    QMessageBox::critical(nullptr, "BlockPad update",mess);
    ui->pushButtonUpdate->setEnabled(true);
}

bool BlockPad::needSaving()
{
    return ui->pushButtonSave->isEnabled();
}

void BlockPad::slotPasswGenClicked()
{
    if(genPasswWgt.isNull())
    {
        genPasswWgt = new GeneratePassword(nullptr);
        genPasswWgt->show();
    }
}

void BlockPad::slotSettingsClicked()
{
    if(setWgt.isNull())
    {
        setWgt = new SettingsWgt(nullptr);
        setWgt->show();
        connect(setWgt, &SettingsWgt::sigScreenLock_Time,
                this, &BlockPad::sigScreenLock_Time);
        connect(setWgt, &SettingsWgt::sigPasswordVisible,
                ui->tableWidgetAccounts, &TableWidgetAccounts::slotAllwaysChecked);
    }
}

void BlockPad::Init()
{
    slotLoadDecrypt();
    slotSaveEncrypt();
    ui->codeEdit->setFocus();
    bool noUpdate = settings.value("noUpdating").toBool();
    if(!noUpdate)
        QtConcurrent::run(this, &BlockPad::checkUpdates, false);
    else
        ui->pushButtonUpdate->setEnabled(true);
}

void BlockPad::closeChildWgts()
{
    if(!setWgt.isNull())
        setWgt->close();
    if(!genPasswWgt.isNull())
        genPasswWgt->close();
}

void BlockPad::slotCompleteRowClicked()
{
    if(ui->tabWidget->currentWidget() == ui->CoinRecords)
    {
        ui->tableWidgetCoinRecords->slotCompletingRow();
    }
    if(ui->tabWidget->currentWidget() == ui->Accounts)
    {
        ui->tableWidgetAccounts->slotCompletingRow();
    }
}

void BlockPad::slotRowSuccessfullyCompleted()
{
    slotSaveEncrypt();
}

void BlockPad::slotCurrentWgtChanged()
{
    auto buttons = ui->ToolsWgt->findChildren<QPushButton *>();
    foreach(auto button, buttons)
    {
        button->show();
    }
    if(ui->tabWidget->currentWidget() == ui->Development)
    {
        ui->pushButtonCompleteRow->hide();
    }
    if(ui->tabWidget->currentWidget() == ui->Accounts)
    {
        ui->pushButtonSave->hide();
        auto item = ui->tableWidgetAccounts->item(0,0);
        ui->tableWidgetAccounts->setCurrentItem(item);
        ui->tableWidgetAccounts->editItem(item);
    }
    if(ui->tabWidget->currentWidget() == ui->CoinRecords)
    {
        ui->pushButtonSave->hide();
        auto item = ui->tableWidgetCoinRecords->item(0,1);
        ui->tableWidgetCoinRecords->setCurrentItem(item);
        ui->tableWidgetCoinRecords->editItem(item);
    }
}

void BlockPad::slotSaveEncrypt()
{
    QByteArray allData;
    //fill allData
    {
        //email
        {
            auto email = qApp->property(defEmailProperty).toString();
            int size_ = email.size();
            allData.append((const char *)&size_, sizeof(int));
            allData.append(email);
        }
        //password
        {
            auto password = qApp->property(defPasswordProperty).toString();
            int size_ = password.size();
            allData.append((const char *)&size_, sizeof(int));
            allData.append(password);
        }
        //table coin records
        allData.append(ui->tableWidgetCoinRecords->dataToEncrypt());
        //table accounts
        allData.append(ui->tableWidgetAccounts->dataToEncrypt());
        //cryptopad
        allData.append(ui->codeEdit->dataToEncrypt());
        auto hash = QMessageAuthenticationCode::hash(allData,
                                                     (QByteArray)defHashKey,QCryptographicHash::Sha256);
        int size = hash.size();
        allData.prepend(hash);
        allData.prepend((const char *)&size, sizeof(int));
    }
    QByteArray cryptoAllData;
    //fill cryptoAllData
    {
        Crypto c =Crypto::Instance();
        c.encrypt(allData,cryptoAllData);
    }
    QString fileName = qApp->property(defFileProperty).toString();

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(nullptr, windowTitle(), "Can not open file to write - " + fileName);
        return;
    }
    file.write(cryptoAllData);
    file.close();
    ui->pushButtonSave->setEnabled(false);
}

void BlockPad::slotBlockPadNewChanges()
{
    ui->pushButtonSave->setEnabled(true);
}

void BlockPad::slotLoadDecrypt()
{
    QByteArray allData;
    QString fileName = qApp->property(defFileProperty).toString();
    //fill allData
    {
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly))
        {
            //QMessageBox::critical(this, "BlockPad", "Can not open file to read - " + fileName);
            return;
        }
        allData = file.readAll();
        file.close();
    }
    QByteArray allDecryptoData;
    //fill allDecryptoData
    {
        Crypto c =Crypto::Instance();
        c.decrypt(allData,allDecryptoData);
    }
    //parse allDecryptoData
    {
        int pos = 0;
        //check hash
        {
            int size = *((int *)allDecryptoData.mid(pos, sizeof(int)).data());
            pos+=sizeof(int);
            auto hashLoad = allDecryptoData.mid(pos, size);
            pos+=size;
            auto hash = QMessageAuthenticationCode::hash(allDecryptoData.mid(pos),
                                                         (QByteArray)defHashKey,QCryptographicHash::Sha256);
            if(hash!= hashLoad)
            {
                QMessageBox::critical(nullptr, windowTitle(), "File " + fileName + " was damaged");
                return;
            }
        }
        //email
        {
            int size = *((int *)allDecryptoData.mid(pos, sizeof(int)).data());
            pos+=sizeof(int);
            QString email = allDecryptoData.mid(pos, size);
            pos+=size;
            //qApp->setProperty(defEmailProperty, email);
        }
        //password
        {
            int size = *((int *)allDecryptoData.mid(pos, sizeof(int)).data());
            pos+=sizeof(int);
            QString password = allDecryptoData.mid(pos, size);
            pos+=size;
            //qApp->setProperty(defPasswordProperty, password);
        }
        ui->tableWidgetCoinRecords->slotLoadData(allDecryptoData, pos);
        ui->tableWidgetAccounts->slotLoadData(allDecryptoData, pos);
        ui->codeEdit->slotLoadData(allDecryptoData, pos);
    }
}

BlockPad::~BlockPad()
{
    delete ui;
}
