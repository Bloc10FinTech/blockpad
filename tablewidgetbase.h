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
protected:
    virtual void addRow(QStringList initTexts = QStringList());
    QSet <int> neverEditableColumns;
private:
    void lockedRow(int iR);
    bool completingRow(bool clickButton = false);
public slots:
    void slotCompletingRow();
protected slots:
    void slotFinishEditing();
    void slotEditedShortcut();
signals:
    void sigCompetingRow();
};

#endif // TABLEWIDGETCP_H
