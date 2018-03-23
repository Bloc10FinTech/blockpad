#ifndef BlockPad_H
#define BlockPad_H

#include <QWidget>
#include <QSettings>
#include <QPointer>
#include "tablewidgetaccounts.h"
#include "tablewidgetcoinrecords.h"
#include "settingswgt.h"
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
    QPointer<SettingsWgt> setWgt;
private:
    Ui::BlockPad *ui;
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
signals:
    void sigScreenLock_Time(bool on);
};

#endif // BlockPad_H
