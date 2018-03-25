#include "tablewidgetbase.h"
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QDebug>
#include <QKeyEvent>
#include <QShortcut>
#include <QIcon>
#include <QMessageBox>
#include <QCursor>
#include <QPoint>
#include <QApplication>

TableWidgetBase::TableWidgetBase(QWidget *parent):
    QTableWidget(parent)
{
    setIconSize(QSize(15,15));
    //connect signals/slots
    {
        connect(this->itemDelegate(), &QAbstractItemDelegate::closeEditor,
                this, &TableWidgetBase::slotFinishEditing);

        QShortcut * shortcut = new QShortcut(QKeySequence(tr("Ctrl+Y")),
                                             this);
        connect(shortcut, &QShortcut::activated,
                this, &TableWidgetBase::slotEditedShortcut);
    }
    setSelectionMode(QAbstractItemView::NoSelection);
}


void TableWidgetBase::addRow(QStringList initTexts)
{
    qDebug() << "TableWidgetBase::addRow";
}

void TableWidgetBase::slotLoadData(QByteArray allLoadData, int & pos)
{
    setRowCount(0);
    clearContents();
    int rows = 0;
    //rows
    {
        rows = *((int *)allLoadData.mid(pos, sizeof(int)).data());
        pos+=sizeof(int);
    }
    QList<QStringList> listRows;
    for(int iR=0; iR<rows; iR++)
    {
        QStringList listTexts;
        for(int iC=0; iC<columnCount(); iC++)
        {
            int size = *((int *)allLoadData.mid(pos, sizeof(int)).data());
            pos+=sizeof(int);
            QString text = allLoadData.mid(pos, size);
            pos+=size;
            listTexts << text;
        }
        listRows.append(listTexts);
    }
    for(int iR=rows-1; iR>=0; iR--)
    {
        QStringList listTexts = listRows[iR];
        addRow(listTexts);
        lockedRow(0);
    }
    addRow();
}

QByteArray TableWidgetBase::dataToEncrypt()
{
    QByteArray res;

    int rows =rowCount()-1;
    res.append((const char *)&rows, sizeof(int));

    for(int iR=1; iR<rowCount(); iR++)
    {
        for(int iC=0; iC<columnCount(); iC++)
        {
            QString text;
            if(item(iR, iC))
                text = item(iR, iC)->text();
            else
                text = cellWidget(iR, iC)->property("text").toString();
            int size = text.size();
            res.append((const char *)&size, sizeof(int));
            res.append(text);
        }
    }
    return res;
}

void TableWidgetBase::slotEditedShortcut()
{
    int curRow = currentRow();
    if(curRow >= 0)
    {
        for(int iCol=0; iCol<columnCount(); iCol++)
        {
            if(!neverEditableColumns.contains(iCol))
            {
                if(item(curRow,iCol))
                {
                    auto currentFlags = item(curRow,iCol)->flags();
                    item(curRow,iCol)->setFlags(currentFlags | Qt::ItemIsEditable);
                    item(curRow,iCol)->setBackgroundColor(Qt::white);
                    item(curRow,iCol)->setIcon(QIcon());
                    item(curRow,iCol)->setToolTip("");
                }
                else
                {
                    cellWidget(curRow,iCol)->setProperty("locked", false);
                }
            }
        }
    }
}

void TableWidgetBase::lockedRow(int iR)
{
    for(int iCol=0; iCol<columnCount(); iCol++)
    {
        if(item(iR,iCol))
        {
            auto currentFlags = item(iR,iCol)->flags();
            item(iR,iCol)->setFlags(currentFlags & (~Qt::ItemIsEditable));
            item(iR,iCol)->setBackgroundColor(defColorNoEditable);
            item(iR,iCol)->setIcon(QIcon("://Icons/locked.png"));
            if(!neverEditableColumns.contains(iCol))
            {
#if defined(WIN32) || defined(WIN64)
                item(iR,iCol)->setToolTip("to make editable item click it and input Ctrl+Y");
#endif
#ifdef __APPLE__
                item(iR,iCol)->setToolTip("to make editable item click it and input Cmd+Y");
#endif
            }
        }
        else
        {
            cellWidget(iR,iCol)->setProperty("locked", true);
        }
    }

}

void TableWidgetBase::slotCompletingRow()
{
    completingRow(true);
}

bool TableWidgetBase::completingRow(bool clickButton)
{
    auto curRow = this->currentRow();
    auto bAllData = true;
    for(int iCol=0; iCol<columnCount(); iCol++)
    {
        if(item(curRow, iCol))
        {
            if(!item(curRow, iCol)->flags().testFlag(Qt::ItemIsEditable))
            {
                continue;
            }
            auto text = item(curRow, iCol)->text();
            if(text == "")
            {
                bAllData = false;
                if(clickButton)
                {
                    QMessageBox::warning(this, "BlockPad", "Please fill in all the boxes!");
                    setCurrentIndex(model()->index(curRow, iCol));
                }
                return bAllData;
            }
        }
        else
        {
            if(cellWidget(curRow, iCol)->property("text").toString() == "")
            {
                bAllData = false;
                if(clickButton)
                {
                    QMessageBox::warning(this, "BlockPad", "Please fill in all the boxes!");
                    cellWidget(curRow, iCol)->setFocus();
                }
                return bAllData;
            }
        }
    }
    if(bAllData)
    {
        lockedRow(curRow);
        if(curRow == 0)
            addRow();
        emit sigCompetingRow();
    }
    return bAllData;
}

TableWidgetBase::~TableWidgetBase()
{
    clear();
}

void TableWidgetBase::slotFinishEditing()
{
    qDebug() << "slotFinishEditing";
    auto curColumn = this->currentColumn();
    auto curRow = this->currentRow();
    auto curItem = item(curRow, curColumn);
    bool bCompleteRow = false;
    if(curRow == 0 && (this->hasFocus() || curItem == nullptr))
    {
        bCompleteRow = completingRow();
    }
    if(!bCompleteRow)
    {
        if(curColumn < columnCount()-1 && this->hasFocus())
        {
            if(cellWidget(curRow, curColumn+1))
                cellWidget(curRow, curColumn+1)->setFocus();
            setCurrentIndex(model()->index(curRow, curColumn+1));
        }
    }
}
