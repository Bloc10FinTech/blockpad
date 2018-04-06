#ifndef REGISTER_H
#define REGISTER_H

#include <QWidget>
#include <QSettings>
#include "crypt.h"
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
    ModeRegistr mode{New};
    ModeRegistr prevMode{New};
    QSettings settings;
    void send2FA();
    void setMode(ModeRegistr newMode);
    void login2FA();
    void sendEmailToGetResponse();
    void OpenFile(QString blockpad);
private slots:
    void slotCreateNewBlockPad();
    void slotLoginClicked();
    void slotOpenFile();
    void slotFinishEditingLogin();
    void slotFinishEditingPassword();
    void slotFinishEditingCode2FA();
    void slotHelloLinkActivated(QString link);
protected:
signals:
    void successRegister();
    void successUnlocked();
};

#endif // REGISTER_H
