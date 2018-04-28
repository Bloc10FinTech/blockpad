#ifndef BlockPad_H
#define BlockPad_H

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QProgressDialog>
#include <QWidget>
#include <QPointer>
#include <QSettings>
#include "tablewidgetaccounts.h"
#include "tablewidgetcoinrecords.h"
#include "settingswgt.h"
#include "onetimepadgeneratorwgt.h"
#include "generatepassword.h"
#include "activatewgt.h"
#include "highlighter.h"
#include <QPrinter>
#include <QUrl>
#include "webBrowser/browser.h"

#include <QGraphicsPixmapItem>

class QListWidgetItem;
class QWebEngineView;

namespace Ui {
class BlockPad;
}

class BlockPad : public QWidget
{
    Q_OBJECT

public:
    explicit BlockPad(QWidget *parent = 0);
    ~BlockPad();
    void Init();
    bool needSaving();
    void closeSeparateWgts();
private:
    Ui::BlockPad *ui;
    QPointer<SettingsWgt> setWgt;
    QPointer<GeneratePassword> genPasswWgt;
    QPointer<OneTimePadGeneratorWgt> oneTimePadGenWgt;
    QPointer<QProgressDialog> progressUpdates;
    QPointer<QNetworkReply> updatingReply;
    QPointer<ActivateWgt> activLicenseWgt;
    QSettings settings;
    QNetworkAccessManager * nam;
    QNetworkAccessManager * namUpdate;
    void checkUpdates(bool bManually = false);
    void descriptionVersion(QString link, QString version, bool bManually);
    void downloadUpdateVersion(QString link, QString version);
    enum TypeRequest {CheckUpdate, DescriptionUpdate, DownloadUpdate};
    bool bRightButtonFiles {false};
    void documentChanged(QString nameDocument);
    int currentTimeId {-1};
    Browser browser;
    BrowserWindow * web_browserWindow {nullptr};
    void successActivation(bool bSuccess);
    QWebEngineView * webEngineView;
    //print data
    void renderHeader(QPainter &painter, QString header,
                      const QRectF& textRect, qreal footerHeight, int pageNumber);
    void printDocument(QPainter* painter, QPrinter& printer, QTextDocument* doc, int &numPage);
    void paintPage(int pageNumber, int pageCount,
                          QPainter* painter, QTextDocument* doc,
                          const QRectF& textRect, qreal footerHeight, int beginNamePage);
    double mmToPixels(QPrinter& printer, int mm);
    const int textMargins = 12; // in millimeters
    const int borderMargins = 10; // in millimeters
protected:
    void timerEvent(QTimerEvent *event);
public slots:
    void slotLoadDecrypt();
    void slotSaveEncrypt();
    void activateWidgets(bool bOn);
private slots:
    void slotCompleteRowClicked();
    void slotRemoveRowClicked();
    void slotSettingsClicked();
    void slotRowSuccessfullyCompleted();
    void slotCurrentWgtChanged();
    void slotBlockPadNewChanges();
    void slotPasswGenClicked();
    void slotActivateClicked();
    void slotFontSizeChanged(int pointSize);
    void slotReplyFinished(QNetworkReply *reply);
    void slotPrintClicked();
    void slotOneTimePadGeneratorClicked();
    void slotAddBlockPadFile();
    void slotDeleteBlockPadFile();
    void slotRenameBlockPadFile();
    void slotFileClicked(QListWidgetItem *item);
    void slotFilesContextMenu(QPoint);
    void slotFilesItemFinishEditing();
    void slotUrlClicked(QUrl url);
    //updates
    void slotUpdateAvailable(QString link, QString version,
                             QString description,  bool bManually = false);
    void slot_No_UpdateAvailable();
    void slotErrorUpdateParsing();
    void slotUpdateClicking();
    void slotDownloadUpdateFinished(QNetworkReply *reply);
    void slotDownloadUpdateProgress(qint64 bytesReceived, qint64 bytesTotal);
    void slotCheckUpdateFinished(QNetworkReply *reply);
    void slotDescriptionFinished(QNetworkReply *reply);
signals:
    void sigScreenLock_Time(int time);
    void sigUpdateAvailable(QString link, QString version, QString description, bool bManually);
    void sigErrorParsing();
    void sig_No_UpdateAvailable();
    void sigNewCryptoPrices();
};

#endif // BlockPad_H
