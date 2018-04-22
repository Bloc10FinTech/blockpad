#include "tablewidgetcoinrecords.h"
#include <QDebug>
#include <QHeaderView>
#include <QDateTime>
#include "passwordwidget.h"

TableWidgetCoinRecords::TableWidgetCoinRecords(QWidget *parent):
    TableWidgetBase(parent)
{
    QStringList list;
    for(int i_Column=0; i_Column<columnsCR::N_Columns;i_Column++)
    {
        switch(i_Column)
        {
            case columnsCR::Time:
            {
                list<< "Time";
            }
            break;
            case columnsCR::BlockAdddres:
            {
                list<< "Block Address";
            }
            break;
            case columnsCR::From:
            {
                list<< "From";
            }
            break;
            case columnsCR::To:
            {
                list<< "To";
            }
            break;
            case columnsCR::Notes:
            {
                list<< "Notes";
            }
            break;
        }
    }
    setColumnCount((int)columnsCR::N_Columns);
    setHorizontalHeaderLabels(list);
    {
        QHeaderView * HeaderView = horizontalHeader();
        HeaderView->setSectionResizeMode(QHeaderView::Stretch);
        HeaderView->setMinimumSectionSize(200);
        HeaderView->setSectionResizeMode(columnsCR::Time, QHeaderView::Fixed);
        HeaderView->resizeSection(columnsCR::Time, 160);
    }
    addRow();
}

TableWidgetCoinRecords::~TableWidgetCoinRecords()
{

}

void TableWidgetCoinRecords::addRow(QStringList initTexts)
{
    insertRow(0);
    for(int i=0; i<columnCount(); i++)
    {
        //Time
        if(i == columnsCR::Time)
        {
            QTableWidgetItem * it = new QTableWidgetItem();
            if(!initTexts.isEmpty())
                it->setText(initTexts[i]);
            it->setFlags(it->flags() & (~Qt::ItemIsEditable));
            it->setBackgroundColor(defColorNoEditable);
            it->setIcon(QIcon("://Icons/locked.png"));
            neverEditableColumns.insert(columnsCR::Time);
            setItem(0, i,it);
        }
        else
        {
            PasswordWidget * wgt;
            wgt = new PasswordWidget(this, true);
            connect(wgt, &PasswordWidget::enterLineEditPressed,
                    this, &TableWidgetCoinRecords::slotFinishEditing);
            connect(wgt, &PasswordWidget::focusIn,
                    this, &TableWidgetCoinRecords::slotFocusInPassword);
            connect(wgt, &PasswordWidget::clickedToChild,
                    this, &TableWidgetCoinRecords::slotClickedPasswordChild);
            if(!initTexts.isEmpty())
                wgt->setText(initTexts[i]);
            setCellWidget(0, i,wgt);
        }
    }
    setCurrentIndex(model()->index(0, 1));
    if(cellWidget(0, 1))
        cellWidget(0,1)->setFocus();
    if(rowCount() > 1 && initTexts.isEmpty())
    {
        item(1, columnsCR::Time)
                ->setText(QLocale("en_EN").toString(QDateTime::currentDateTime(), "hh:mm ap M/d/yyyy"));
    }
}
