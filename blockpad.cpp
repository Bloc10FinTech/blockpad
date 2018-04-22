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
#include <QNetworkReply>
#include <QProgressDialog>
#include <QNetworkAccessManager>
#include <QCoreApplication>
#include <QProcess>
#include <QDesktopWidget>
#include <QMetaEnum>
#include <QtConcurrent>
#include <QPrinter>
#include <QPrintDialog>
#include <QPaintEngine>
#include <QInputDialog>
#include <QGraphicsScene>
#include <QMenu>
#include <QLabel>
#include <QDateTime>
#include "tablePrinter/tableprinter.h"

#define defReplyType "ReplyType"
#define defDefaultNameFile "new "

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
    slotCurrentWgtChanged();
    //highlighter = new Highlighter(ui->codeEdit->document());
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
    nam = new QNetworkAccessManager(this);
    //signals-slots connects
    {
        connect(ui->pushButtonCompleteRow, &QPushButton::clicked,
                this, &BlockPad::slotCompleteRowClicked);
        connect(ui->pushButtonUpdate, &QPushButton::clicked,
                this, &BlockPad::slotUpdateClicking);
        connect(ui->pushButtonSettings, &QPushButton::clicked,
                this, &BlockPad::slotSettingsClicked);
        connect(ui->pushButtonPrint, &QPushButton::clicked,
                this, &BlockPad::slotPrintClicked);
        connect(ui->pushButtonAddFile, &QPushButton::clicked,
                this, &BlockPad::slotAddBlockPadFile);
        connect(ui->pushButton1TimePad, &QPushButton::clicked,
                this, &BlockPad::slotOneTimePadGeneratorClicked);
        connect(ui->pushButtonGeneratePassword, &QPushButton::clicked,
                this, &BlockPad::slotPasswGenClicked);
        connect(ui->pushButtonSave, &QPushButton::clicked,
                this, &BlockPad::slotSaveEncrypt);
        connect(ui->pushButtonRemoveRow, &QPushButton::clicked,
                this, &BlockPad::slotRemoveRowClicked);
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
        connect(nam, &QNetworkAccessManager::finished,
                this, &BlockPad::slotReplyFinished);
        connect(this, &BlockPad::sig_No_UpdateAvailable,
                this, &BlockPad::slot_No_UpdateAvailable);
        connect(this, &BlockPad::sigErrorParsing,
                this, &BlockPad::slotErrorUpdateParsing);
        connect(ui->listWidgetFiles, &QListWidget::itemClicked,
                this, &BlockPad::slotFileClicked);
        connect(ui->listWidgetFiles, &QListWidget::currentTextChanged,
                this, &BlockPad::slotFilesItemFinishEditing);
        connect(ui->listWidgetFiles, SIGNAL(customContextMenuRequested(QPoint)),
                this, SLOT(slotFilesContextMenu(QPoint)));
    }
    //change font size
    {
        auto fontSize = settings.value("FontSize").toInt();
        if(fontSize == 0)
        {
        #if defined(WIN32) || defined(WIN64)
            fontSize = 12;
        #endif
        #ifdef __APPLE__
            fontSize = 14;
        #endif
        }
        slotFontSizeChanged(fontSize);
    }
    //add time
    currentTimeId = startTimer(1000);
    {
        QLabel* pLabelTime = new QLabel(this);
        pLabelTime->setText("the time is " +
        QLocale("en_EN").toString(QDateTime::currentDateTime(), "hh:mm ap M/d/yyyy")
        //rewrite with qsplitter
        + "    ");
        ui->tabWidget->setCornerWidget(pLabelTime, Qt::TopRightCorner);
    }
}

void BlockPad::timerEvent(QTimerEvent *event)
{
    if(currentTimeId == event->timerId())
    {
        QLabel * lbl = qobject_cast<QLabel *>(ui->tabWidget->cornerWidget());
        if(lbl != nullptr)
        {
            lbl->setText("the time is " +
                         QLocale("en_EN").toString(QDateTime::currentDateTime(), "hh:mm ap M/d/yyyy")
                         //rewrite with qsplitter
                         + "    ");
        }
    }
}

void BlockPad::slotFilesContextMenu(QPoint pos)
{
    // Handle global position
    QPoint globalPos = ui->listWidgetFiles->mapToGlobal(pos);
    auto item = ui->listWidgetFiles->itemAt(pos);
    if(item)
    {
        // Create menu and insert some actions
        QMenu myMenu;
        myMenu.addAction("Add", this, SLOT(slotAddBlockPadFile()));
        myMenu.addAction("Erase",  this, SLOT(slotDeleteBlockPadFile()));
        myMenu.addAction("Rename",  this, SLOT(slotRenameBlockPadFile()));
        // Show context menu at handling position
        myMenu.exec(globalPos);
    }
}

void BlockPad::slotFileClicked(QListWidgetItem *item)
{
    auto nameDocument = item->text();
    documentChanged(nameDocument);
}


void BlockPad::slotDeleteBlockPadFile()
{
    auto row = ui->listWidgetFiles->currentRow();
    QString text = ui->listWidgetFiles->item(row)->text();
    if(text == "main")
    {
        QMessageBox::warning(this, windowTitle(),
                             "You can not delete file \"main\"");
        return;
    }
    if(QMessageBox::Yes == QMessageBox::question(this, windowTitle(),
                          "Are you sure that you want to delete the file " + text + " ?"))
    {
        auto item = ui->listWidgetFiles->takeItem(row);
        delete item;
        auto docs = ui->codeEdit->getAllDocuments();
        if(docs[text] == ui->codeEdit->document())
        {
            documentChanged("main");
        }
        ui->codeEdit->removeDocument(text);
    }
}

void BlockPad::slotRenameBlockPadFile()
{
    QListWidgetItem *item = ui->listWidgetFiles->currentItem();
    if(item->text() == "main")
    {
        QMessageBox::warning(this, windowTitle(),
                             "You can not rename file \"main\"");
        return;
    }
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    ui->listWidgetFiles->editItem(item);
}

void BlockPad::slotFilesItemFinishEditing()
{
    QListWidgetItem *item = ui->listWidgetFiles->currentItem();
    item->setFlags(item->flags() & (~Qt::ItemIsEditable));
}

void BlockPad::slotOneTimePadGeneratorClicked()
{
    if(oneTimePadGenWgt.isNull())
    {
        oneTimePadGenWgt = new OneTimePadGeneratorWgt(nullptr);
        oneTimePadGenWgt->show();
    }
}

void BlockPad::slotReplyFinished(QNetworkReply *reply)
{
    TypeRequest type = (TypeRequest)reply->property(defReplyType).toInt();

    if(TypeRequest::CheckUpdate == type)
        slotCheckUpdateFinished(reply);
    if(TypeRequest::DescriptionUpdate == type)
        slotDescriptionFinished(reply);
}

void BlockPad::slotCheckUpdateFinished(QNetworkReply *reply)
{
    auto error = reply->error();
    if(error != QNetworkReply::NoError)
    {
        auto data = QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(error);
        QMessageBox::critical(nullptr, windowTitle(), data);
        ui->pushButtonUpdate->setEnabled(true);
        return;
    }
    auto data = (QString)(reply->readAll());
    bool bManually = reply->property("Manually").toBool();
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
            if(latestVersion != defVersionApplication)
                descriptionVersion(downloadLink, latestVersion, bManually);
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

void BlockPad::descriptionVersion(QString link,
                                  QString version,
                                  bool bManually)
{
    QUrl url;
#if defined(WIN32) || defined(WIN64)
    url = QUrl("https://bintray.com/bloc10fintech/BlockPad/BlockPad_stable_windows/" +version);
#endif
#ifdef __APPLE__
    url = QUrl("https://bintray.com/bloc10fintech/BlockPad/BlockPad_stable_mac/" +version);
#endif
    QNetworkRequest request(url);
    auto reply = nam->get(request);
    reply->setProperty("Manually", bManually);
    reply->setProperty("Link", link);
    reply->setProperty("Version", version);
    reply->setProperty(defReplyType, TypeRequest::DescriptionUpdate);
}

void BlockPad::slotDescriptionFinished(QNetworkReply *reply)
{
    auto error = reply->error();
    if(error != QNetworkReply::NoError)
    {
        auto data = QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(error);
        QMessageBox::critical(nullptr, windowTitle(), data);
        ui->pushButtonUpdate->setEnabled(true);
        return;
    }
    auto data = (QString)(reply->readAll());
    bool bManually = reply->property("Manually").toBool();
    QString latestVersion = reply->property("Version").toString();
    QString downloadLink = reply->property("Link").toString();
    QString descriptionNewVersion;
    //fill descriptionNewVersion
    {
        auto indexBlockDescription = data.indexOf("<div id=\"block-avatar-description\">");
        if(indexBlockDescription != -1)
        {
            auto indexStartText = data.indexOf("<div class=\"description-text\">", indexBlockDescription);
            if(indexStartText != -1)
            {
                auto indexEndText = data.indexOf("</div>", indexStartText);
                if(indexEndText != -1)
                {
                    descriptionNewVersion = data.mid(indexStartText,
                                                     indexEndText-indexStartText)
                            .remove("<div class=\"description-text\">");
                }
            }
        }
    }
    emit sigUpdateAvailable(downloadLink, latestVersion,
                            descriptionNewVersion, bManually);
}

void BlockPad::slotUpdateClicking()
{
    checkUpdates(true);
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

    QFile file;
#if defined(WIN32) || defined(WIN64)
    file.setFileName(Utilities::filesDirectory() + "/SetupBlockPad.exe");
#endif
#ifdef __APPLE__
    file.setFileName(Utilities::filesDirectory() + "/BlockPad.zip");
#endif
    file.open(QIODevice::WriteOnly);
    file.write(dataReply);
    file.close();

    QMessageBox mesBox;
    mesBox.setText("BlockPad is opened\r\nUpdater will close it in order to process installiation\r\nContinue?");
    mesBox.setWindowTitle("BlockPad update");

    QPushButton * yesButton = mesBox.addButton("Yes", QMessageBox::YesRole);
    yesButton->setDefault(true);
    QPushButton * noButton = mesBox.addButton("No", QMessageBox::NoRole);

    mesBox.setIcon(QMessageBox::Question);
    mesBox.exec();
    if (yesButton == mesBox.clickedButton())
    {
        QProcess procFinishUpdate;
#if defined(WIN32) || defined(WIN64)
        procFinishUpdate.startDetached(Utilities::filesDirectory()+ "/SetupBlockPad.exe");
#endif
#ifdef __APPLE__
        procFinishUpdate.startDetached("open -a \"" + Utilities::filesDirectory()
                                       + "/UpdateBlockPad.app\" --args \""
                                       + Utilities::filesDirectory() + "\" \""
                                       + Utilities::applicationPath() + "\"");
#endif
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

void BlockPad::slotUpdateAvailable(QString link, QString version,
                                   QString description, bool bManually)
{   
    QMessageBox mesBox;
    mesBox.setText("An update package is available, do you want to download it?\r\n" + description.remove("<br>").remove("<br/>"));
    mesBox.setWindowTitle("BlockPad update");

    QPushButton * yesButton = mesBox.addButton("Yes", QMessageBox::YesRole);
    mesBox.setDefaultButton(yesButton);
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
    QUrl url;
#if defined(WIN32) || defined(WIN64)
    url = QUrl("https://bintray.com/package/info/bloc10fintech/BlockPad/BlockPad_stable_windows");
#endif
#ifdef __APPLE__
    url = QUrl("https://bintray.com/package/info/bloc10fintech/BlockPad/BlockPad_stable_mac");
#endif
    QNetworkRequest request(url);
    auto reply = nam->get(request);
    reply->setProperty("Manually", bManually);
    reply->setProperty(defReplyType, TypeRequest::CheckUpdate);
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
        connect(setWgt, &SettingsWgt::sigFontSizeChanged,
                this, &BlockPad::slotFontSizeChanged);
        connect(setWgt, &SettingsWgt::sigHighlightingCode,
                ui->codeEdit, &CodeEditor::slotHighlightingCode);
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
    //dont know width of splitter in this place - it is reason why
    //we get width of code editor 999999 (Of course this is more than required)
    ui->splitterBlockPad->setSizes(QList<int>() <<999999<<100);
    if(!noUpdate)
        checkUpdates();
        //    QtConcurrent::run(this, &BlockPad::checkUpdates, false);
    else
        ui->pushButtonUpdate->setEnabled(true);
}

void BlockPad::closeSeparateWgts()
{
    if(!setWgt.isNull())
        setWgt->close();
    if(!oneTimePadGenWgt.isNull())
        oneTimePadGenWgt->close();
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

void BlockPad::activateWidgets(bool bOn)
{
    ui->widgetTicker->slotStart(bOn);
}

void BlockPad::slotRowSuccessfullyCompleted()
{
    slotSaveEncrypt();
}

void BlockPad::slotPrintClicked()
{
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle(tr("Print Document"));
    if (dialog.exec() == QDialog::Accepted && printer.isValid()) {
        int iNumPage = 1;

        QPainter painter;
        QSizeF pageSize = printer.pageRect().size(); // page size in pixels
        const double tm = mmToPixels(printer, textMargins);
        const qreal footerHeight = painter.fontMetrics().height();
        const QRectF textRect(tm, tm, pageSize.width() - 2 * tm,
                              pageSize.height() - 2 * tm - footerHeight);

        painter.begin(&printer);//p is my QPrinter
        painter.setRenderHint(QPainter::Antialiasing, true);

        auto allDocuments = ui->codeEdit->getAllDocuments();

        foreach(QString nameDoc, allDocuments.keys())
        {
            renderHeader(painter, "BlockPad ( " + nameDoc + " )",
                         textRect, footerHeight, iNumPage);
            printer.newPage();
            iNumPage++;

            //auto doc = ui->codeEdit->document()->clone(this);
            auto doc = allDocuments[nameDoc]->clone(this);
            Highlighter *highlighter = new Highlighter(doc);
            highlighter->rehighlight();
            QTextOption opt = doc->defaultTextOption();
            opt.setWrapMode(QTextOption::WrapAnywhere);
            doc->setDefaultTextOption(opt);
            printDocument(&painter, printer, doc, iNumPage);

            printer.newPage();
            doc->deleteLater();
        }
        //iNumPage++;
        renderHeader(painter, "CoinRecords",
                     textRect, footerHeight, iNumPage);

        printer.newPage();
        iNumPage++;

        //{
            TablePrinter tablePrinter(&painter, &printer);
            //tablePrinter.setPageMargin(tm,tm, tm, tm);
            QVector<int> columnStretch = QVector<int>() << 1 << 1 << 1 << 1 << 1;
            QVector<QString> headers = QVector<QString>() << "Time" << "Block Address"
                                                          << "From" << "To" << "Notes";
            if(!tablePrinter.printTable(ui->tableWidgetCoinRecords,
                                        columnStretch, textRect,
                                        footerHeight, iNumPage,
                                        headers)) {
                qDebug() << tablePrinter.lastError();
            }
        //}
        printer.newPage();

        iNumPage++;

        renderHeader(painter, "Accounts",
                     textRect, footerHeight, iNumPage);

        printer.newPage();

        iNumPage++;

        //{

            QVector<int> columnStretch_ = QVector<int>() << 1 << 1 << 1;
            QVector<QString> headers_ = QVector<QString>() << "Web Site" << "Username"
                                                          << "Password";
            if(!tablePrinter.printTable(ui->tableWidgetAccounts,
                                        columnStretch_, textRect,
                                        footerHeight, iNumPage,
                                        headers_, columnsAccount::WebSite)) {
                qDebug() << tablePrinter.lastError();
            }

        painter.end();
    }
}

double BlockPad::mmToPixels(QPrinter& printer, int mm)
{
    return mm * 0.039370147 * printer.resolution();
}

void BlockPad::printDocument(QPainter* painter, QPrinter& printer,
                             QTextDocument* doc, int & numPage)
{
    doc->documentLayout()->setPaintDevice(&printer);
    doc->setPageSize(printer.pageRect().size());
    QSizeF pageSize = printer.pageRect().size(); // page size in pixels
    // Calculate the rectangle where to lay out the text
    const double tm = mmToPixels(printer, textMargins);
    const qreal footerHeight = painter->fontMetrics().height();
    const QRectF textRect(tm, tm, pageSize.width() - 2 * tm, pageSize.height() - 2 * tm - footerHeight);
    doc->setPageSize(textRect.size());

    const int pageCount = doc->pageCount();
    int beginNamePage =numPage;
    bool firstPage = true;
    for (int pageIndex = 0; pageIndex < pageCount; ++pageIndex) {
        if (!firstPage)
            printer.newPage();

        paintPage(pageIndex, pageCount, painter, doc, textRect, footerHeight,beginNamePage);
        firstPage = false;
    }
    numPage +=pageCount;
}

void BlockPad::paintPage(int pageNumber, int pageCount,
                      QPainter* painter, QTextDocument* doc,
                      const QRectF& textRect, qreal footerHeight, int beginNamePage)
{


    painter->save();

    const QRectF textPageRect(0, pageNumber * doc->pageSize().height(), doc->pageSize().width(), doc->pageSize().height());
    // Translate so that 0,0 is now the page corner
    painter->translate(0, -textPageRect.top());
    // Translate so that 0,0 is the text rect corner
    painter->translate(textRect.left(), textRect.top());
    QAbstractTextDocumentLayout *layout = doc->documentLayout();
    QAbstractTextDocumentLayout::PaintContext ctx;

    painter->setClipRect(textPageRect);
    ctx.clip = textPageRect;
    ctx.palette.setColor(QPalette::Text, Qt::black);
    layout->draw(painter, ctx);
    painter->restore();
    QRectF footerRect = textRect;
    footerRect.setTop(textRect.bottom());
    footerRect.setHeight(footerHeight);
    painter->drawText(footerRect, Qt::AlignVCenter | Qt::AlignRight, QString::number(pageNumber+beginNamePage));

}

void BlockPad::slotRemoveRowClicked()
{
    if(QMessageBox::Yes == QMessageBox::question(this, windowTitle(), "Are you sure that you want delete row?"))
    {
        if(ui->tabWidget->currentWidget() == ui->CoinRecords)
        {
            if(ui->tableWidgetCoinRecords->getRowHighlighting() > 0)
                ui->tableWidgetCoinRecords->removeRow(ui->tableWidgetCoinRecords->getRowHighlighting());
        }
        if(ui->tabWidget->currentWidget() == ui->Accounts)
        {
            if(ui->tableWidgetAccounts->getRowHighlighting() >  0)
                ui->tableWidgetAccounts->removeRow(ui->tableWidgetAccounts->getRowHighlighting());
        }
        slotSaveEncrypt();
    }
}

void BlockPad::slotAddBlockPadFile()
{
    QString defaultNameFile;
    auto allDocuments = ui->codeEdit->getAllDocuments();
    //fill defaultNameFile
    {
        int iNum = 1;
        while(1)
        {
            defaultNameFile = defDefaultNameFile + QString::number(iNum);
            if(allDocuments.contains(defaultNameFile))
                iNum++;
            else
                break;
        }
    }
    bool ok = false;
    QString nameFile = QInputDialog::getText(this, windowTitle(), tr("Name File"),
                                QLineEdit::Normal, defaultNameFile, &ok);

    if(ok && !nameFile.isEmpty())
    {
        if(allDocuments.contains(nameFile))
        {
            int iNum = 1;
            while(1)
            {
                nameFile = nameFile + " " + QString::number(iNum);
                if(allDocuments.contains(nameFile))
                    iNum++;
                else
                    break;
            }
        }
        ui->listWidgetFiles->addItem(nameFile);
        documentChanged(nameFile);
        ui->pushButtonSave->setEnabled(true);
    }
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
        ui->pushButtonRemoveRow->hide();
    }
    if(ui->tabWidget->currentWidget() == ui->Accounts)
    {
        ui->pushButtonSave->hide();
        ui->pushButtonAddFile->hide();
        ui->tableWidgetAccounts->Init();
    }
    if(ui->tabWidget->currentWidget() == ui->CoinRecords)
    {
        ui->pushButtonSave->hide();
        ui->pushButtonAddFile->hide();
        ui->tableWidgetCoinRecords->Init();
    }
}

void BlockPad::slotFontSizeChanged(int pointSize)
{
    auto wgts = findChildren<QWidget *>();
    foreach (QWidget *widget, wgts)
    {
        QFont font = widget->font();
        font.setPointSize(pointSize);
        widget->setFont(font);
    }
    settings.setValue("FontSize", pointSize);
    settings.sync();
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
        int versionProtocol = defVersionEncryptProtocol;
        allData.prepend((const char *)&versionProtocol, sizeof(int));
    }
    QByteArray cryptoAllData;
    QByteArray baIv;
    //fill cryptoAllData
    {
        Crypto c =Crypto::Instance();
        c.encrypt(allData,cryptoAllData, baIv);
        cryptoAllData.prepend(baIv);
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
            ui->listWidgetFiles->addItem("main");
            documentChanged("main");
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
        int versionProtocol = *((int *)allDecryptoData.mid(pos, sizeof(int)).data());
        pos+=sizeof(int);
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
        auto allDocuments = ui->codeEdit->slotLoadData(allDecryptoData, pos);
        ui->listWidgetFiles->addItems(allDocuments.keys());
        QString currentName = allDocuments.firstKey();
        foreach(QString name, allDocuments.keys())
        {
            if(ui->codeEdit->document() == allDocuments[name])
            {
                currentName = name;
                break;
            }
        }
        documentChanged(currentName);
    }
}

void BlockPad::documentChanged(QString nameDocument)
{
    //all other items with default color
    {
        for(int i=0; i<ui->listWidgetFiles->count(); i++)
        {
            auto item = ui->listWidgetFiles->item(i);
            if(item->text() == nameDocument)
                item->setIcon(QIcon("://Icons/Display.png"));
            else
                item->setIcon(QIcon());
        }
    }
    ui->tabWidget->setTabText(ui->tabWidget->indexOf(ui->Development),
                              "BlockPad ( " + nameDocument + " )");
    ui->codeEdit->setCurrentDocument(nameDocument);
}

BlockPad::~BlockPad()
{
    delete ui;
}

void BlockPad::renderHeader(QPainter &painter, QString header,
                            const QRectF &textRect, qreal footerHeight, int pageNumber)
{
    painter.save();
    painter.resetTransform();
    painter.setFont(QFont(painter.font().family(),
                          25,
                          QFont::Bold));
    painter.drawText(painter.window(), Qt::AlignCenter, header);
    //painter.drawRect(boundingRect);
    painter.restore();
    QRectF footerRect = textRect;
    footerRect.setTop(textRect.bottom());
    footerRect.setHeight(footerHeight);
    painter.drawText(footerRect, Qt::AlignVCenter | Qt::AlignRight, QString::number(pageNumber));

}
