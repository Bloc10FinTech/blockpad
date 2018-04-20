#ifndef TABLEWIDGETCP_H
#define TABLEWIDGETCP_H

#include <QObject>
#include<QTableWidget>
#include <QSet>
#include <QStringList>
#include "global.h"

#define defNeverEditableRole Qt::UserRole + 1


class TableWidgetBase : public QTableWidget
{
    Q_OBJECT
public:
    TableWidgetBase(QWidget *parent = Q_NULLPTR);
    ~TableWidgetBase();
    QByteArray dataToEncrypt();
    void slotLoadData(QByteArray allLoadData, int &pos);
    void Init();
protected:
    virtual void addRow(QStringList initTexts = QStringList());
    QSet <int> neverEditableColumns;
    void mouseReleaseEvent(QMouseEvent *event);
    void highlightingLine(int row);
private:
    void lockedRow(int iR);
    bool bClearContents {false};
    bool bNewCol {false};
    bool completingRow(bool clickButton = false);
public slots:
    void slotCompletingRow();
protected slots:
    void slotFinishEditing();
    void slotEditedShortcut();
//    void slotCurrentCellChanged(int currentRow, int currentColumn,
//                                int previousRow, int previousColumn);
    void slotFocusInPassword(QWidget * wgt);
    void slotClickedPasswordChild();
signals:
    void sigCompetingRow();
};

#endif // TABLEWIDGETCP_H
