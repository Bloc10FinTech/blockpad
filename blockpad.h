#ifndef BlockPad_H
#define BlockPad_H

#include <QWidget>
#include <QSettings>
#include <QPointer>
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
    QSettings settings;
    Highlighter *highlighter;
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
signals:
    void sigScreenLock_Time(bool on);
};

#endif // BlockPad_H
