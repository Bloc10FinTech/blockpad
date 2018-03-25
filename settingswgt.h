#ifndef SETTINGSWGT_H
#define SETTINGSWGT_H

#include <QWidget>
#include <QSettings>

namespace Ui {
class SettingsWgt;
}

class SettingsWgt : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWgt(QWidget *parent = 0);
    ~SettingsWgt();
private:
    Ui::SettingsWgt *ui;
    QSettings settings;
private slots:
    void slotFontAppChanged(QString newFamily);
    void slotPasswordVisibleClicked(bool bCheck);
    void slot2FA_On_Clicked(bool bCheck);
    void slotCheckUpdatesStartUp(bool bCheck);
    void slotLockScreen_Time_FinishEditing();
signals:
    void sigScreenLock_Time(int time);
    void sigPasswordVisible(bool on);
};

#endif // SETTINGSWGT_H
