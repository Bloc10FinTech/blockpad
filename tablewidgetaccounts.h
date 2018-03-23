#ifndef TABLEWIDGETACCOUNTS_H
#define TABLEWIDGETACCOUNTS_H
#include "tablewidgetbase.h"
#include <QObject>
enum  columnsAccount {WebSite, Username, Password, N_ColumnsAccount};

class TableWidgetAccounts : public TableWidgetBase
{
    Q_OBJECT
public:
    TableWidgetAccounts(QWidget *parent);
    ~TableWidgetAccounts();
protected:
    void addRow(QStringList initTexts = QStringList());
private:
    bool allwaysVisible {true};
public slots:
    void slotAllwaysChecked(bool allways);
private slots:
    void slotFocusInPassword(QWidget * wgt);
signals:
    void allwaysChecked(bool allways);
};

#endif // TABLEWIDGETACCOUNTS_H
