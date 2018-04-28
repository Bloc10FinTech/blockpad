#ifndef TABLEWIDGETACCOUNTS_H
#define TABLEWIDGETACCOUNTS_H
#include "tablewidgetbase.h"
#include <QObject>
#include <QUrl>
enum  columnsAccount {WebSite, Username, Password, N_ColumnsAccount};

class TableWidgetAccounts : public TableWidgetBase
{
    Q_OBJECT
public:
    TableWidgetAccounts(QWidget *parent);
    ~TableWidgetAccounts();
protected:
    void addRow(QStringList initTexts = QStringList());
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
private:
    bool allwaysVisible {true};
    bool clickHyperLink {false};
public slots:
    void slotAllwaysChecked(bool allways);
private slots:

signals:
    void allwaysChecked(bool allways);
    void sigClickedUrl(QUrl url);
};

#endif // TABLEWIDGETACCOUNTS_H
