#ifndef TABLEWIDGETCOINRECORDS_H
#define TABLEWIDGETCOINRECORDS_H
#include "tablewidgetbase.h"

enum  columnsCR {Time, BlockAdddres, From, To, Notes, N_Columns};

class TableWidgetCoinRecords : public TableWidgetBase
{
    Q_OBJECT
public:
    TableWidgetCoinRecords(QWidget *parent = Q_NULLPTR);
    ~TableWidgetCoinRecords();   
protected:
    void addRow(QStringList initTexts = QStringList());
};

#endif // TABLEWIDGETCOINRECORDS_H
