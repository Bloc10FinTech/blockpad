#ifndef REGISTER_H
#define REGISTER_H

#include <QWidget>
#include <QSettings>
#include "crypt.h"
#include "retreivecloudwgt.h"
#include <QPointer>

namespace Ui {
class Register;
}

class Register : public QWidget
{
    Q_OBJECT

public:
    explicit Register(QWidget *parent = 0);
    ~Register();
    void onLock();
    void Init();
private:
    Ui::Register *ui;
    QList<QPair<QString, QString> > _listEmailPassws;
    enum ModeRegistr {New, OpenBlockPad, modeLock, mode2FA};
    QList<QString> nameFiles;   
    QMap<QString, QString> fileIds;
    ModeRegistr mode{New};
    ModeRegistr prevMode{New};
    QSettings settings;
    bool send2FA();
    void setMode(ModeRegistr newMode);
    void login2FA();
    void sendEmailToGetResponse();
    void OpenFile(QString blockpad);
    int resendId {0};
    int resendTime{60}; //seconds
    QString currentEmail();
    QPointer<RetreiveCloudWgt> retreiveCloudWgt;
private slots:
    void slotCreateNewBlockPad();
    void slotLoginClicked();
    void slotOpenFile();
    void slotFinishEditingLogin();
    void slotFinishEditingPassword();
    void slotFinishEditingCode2FA();
    void slotFinishChoosingId();
    void slotRetreiveCloudClicked();
    void slotHelloLinkActivated(QString link);
protected:
    void timerEvent(QTimerEvent *event);
signals:
    void successRegister();
    void successUnlocked();
};

#endif // REGISTER_H
