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
#include "generatepassword.h"
#include "highlighter.h"


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
    void closeChildWgts();
private:
    Ui::BlockPad *ui;
    QPointer<SettingsWgt> setWgt;
    QPointer<GeneratePassword> genPasswWgt;
    QPointer<QProgressDialog> progressUpdates;
    QPointer<QNetworkReply> updatingReply;
    QSettings settings;
    QNetworkAccessManager * namUpdate;
    QNetworkAccessManager * namCheckUpdates;
    Highlighter *highlighter;
    void checkUpdates(bool bManually = false);
    void downloadUpdateVersion(QString link, QString version);
protected:
public slots:
    void slotLoadDecrypt();
    void slotSaveEncrypt();
private slots:
    void slotCompleteRowClicked();
    void slotSettingsClicked();
    void slotRowSuccessfullyCompleted();
    void slotCurrentWgtChanged();
    void slotBlockPadNewChanges();
    void slotPasswGenClicked();
    void slotFontSizeChanged(int pointSize);

    //updates
    void slotUpdateAvailable(QString link, QString version, bool bManually = false);
    void slot_No_UpdateAvailable();
    void slotErrorUpdateParsing();
    void slotUpdateClicking();
    void slotDownloadUpdateFinished(QNetworkReply *reply);
    void slotDownloadUpdateProgress(qint64 bytesReceived, qint64 bytesTotal);
    void slotCheckUpdateFinished(QNetworkReply *reply);
signals:
    void sigScreenLock_Time(int time);
    void sigUpdateAvailable(QString link, QString version, bool bManually);
    void sigErrorParsing();
    void sig_No_UpdateAvailable();
};

#endif // BlockPad_H
