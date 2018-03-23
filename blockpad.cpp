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
#include "adswebpage.h"
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
            auto allways = !settings.value("MakePasswordsHidden").toBool();
            ui->tableWidgetAccounts->slotAllwaysChecked(!allways);
        }
    }
    //signals-slots connects
    {
        connect(ui->pushButtonCompleteRow, &QPushButton::clicked,
                this, &BlockPad::slotCompleteRowClicked);
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
        //webEngineView->setUrl(QUrl("https://www.google.ru"));
        webEngineView->setFixedHeight(110);
    }
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
        //genPasswWgt->setAttribute(Qt::WA_QuitOnClose, true);
        genPasswWgt->show();
    }
}

void BlockPad::slotSettingsClicked()
{
    if(setWgt.isNull())
    {
        setWgt = new SettingsWgt(nullptr);
        //setWgt->setAttribute(Qt::WA_QuitOnClose, true);
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
        QMessageBox::critical(this, windowTitle(), "Can not open file to write - " + fileName);
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
                QMessageBox::critical(this, windowTitle(), "File " + fileName + " was damaged");
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
