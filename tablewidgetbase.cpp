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
#include <QMetaEnum>
#include <QMenu>
#include <QClipboard>
#include <QStyleFactory>

TableWidgetBase::TableWidgetBase(QWidget *parent):
    QTableWidget(parent)
{
    setIconSize(QSize(15,15));
    //connect signals/slots
    {
        QShortcut * shortcut = new QShortcut(QKeySequence(tr("Ctrl+Y")),
                                             this);
        connect(shortcut, &QShortcut::activated,
                this, &TableWidgetBase::slotEditedShortcut);
    }
    setSelectionMode(QAbstractItemView::NoSelection);
#ifdef __APPLE__
    setStyle(QStyleFactory::create("Windows"));
#endif
}

void TableWidgetBase::addRow(QStringList initTexts)
{
}

void TableWidgetBase::highlightingLine(int row)
{
    rowHighlighting = -1;
    bool editable = false;
    if(row == 0)
        editable =true;
    if(row > 0 && row < rowCount())
    {
        for(int iCol=0; iCol<columnCount(); iCol++)
        {
            if(item(row,iCol))
            {
                if(!item(row,iCol)->flags().testFlag(Qt::ItemIsEditable))
                {
                    item(row,iCol)->setBackgroundColor(Qt::magenta);
                    for(int iR =1; iR<rowCount(); iR++)
                    {
                        if(row != iR &&
                          !item(iR,iCol)->flags().testFlag(Qt::ItemIsEditable))
                        {
                            item(iR,iCol)->setBackgroundColor(defColorNoEditable);
                        }
                    }
                }
                else
                    editable =true;
            }
            else
            {
                if(cellWidget(row,iCol)->property("locked").toBool())
                {
                    cellWidget(row,iCol)->setProperty("highlighted", true);
                    rowHighlighting = row;
                    for(int iR =0; iR<rowCount(); iR++)
                    {
                        if(row != iR &&
                        cellWidget(iR,iCol)->property("locked").toBool())
                        {
                            cellWidget(iR,iCol)->setProperty("highlighted", false);
                        }
                    }
                }
                else
                    editable =true;
            }
        }
    }
    if(editable)
    {
        for(int iCol=0; iCol<columnCount(); iCol++)
        {
            if(item(row,iCol))
            {
                for(int iR =1; iR<rowCount(); iR++)
                {
                    if(!item(iR,iCol)->flags().testFlag(Qt::ItemIsEditable))
                    {
                        item(iR,iCol)->setBackgroundColor(defColorNoEditable);
                    }
                }
            }
            else
            {
                for(int iR =0; iR<rowCount(); iR++)
                {
                    if(cellWidget(iR,iCol)->property("locked").toBool())
                        cellWidget(iR,iCol)->setProperty("highlighted", false);
                }
            }
        }
    }
}

int TableWidgetBase::getRowHighlighting()
{
    return rowHighlighting;
}

void TableWidgetBase::slotLoadData(QByteArray allLoadData, int & pos)
{
    bClearContents = true;
    setRowCount(0);
    clearContents();
    bClearContents = false;
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
    if(rowHighlighting >= 0)
    {
        for(int iCol=0; iCol<columnCount(); iCol++)
        {
            if(!neverEditableColumns.contains(iCol))
            {
                if(item(rowHighlighting,iCol))
                {
                    item(rowHighlighting,iCol)->setFlags(item(rowHighlighting,iCol)->flags().setFlag(Qt::ItemIsEditable, true));
                    item(rowHighlighting,iCol)->setBackgroundColor(Qt::white);
                    item(rowHighlighting,iCol)->setIcon(QIcon());
                    item(rowHighlighting,iCol)->setToolTip("");
                }
                else
                {
                    cellWidget(rowHighlighting,iCol)->setProperty("locked", false);
                    cellWidget(rowHighlighting,iCol)->setProperty("highlighted", false);
                }
            }
            else
            {
                if(item(rowHighlighting,iCol))
                {
                    item(rowHighlighting,iCol)->setBackgroundColor(defColorNoEditable);
                }
                else
                {
                    cellWidget(rowHighlighting,iCol)->setProperty("highlighted", false);
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
            item(iR,iCol)->setFlags(item(iR,iCol)->flags()
                .setFlag(Qt::ItemIsEditable, false)
                .setFlag(Qt::ItemIsSelectable, true));
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

void TableWidgetBase::Init()
{
    if(rowAt(rect().topLeft().y()) == 0)
    {
        //QTableWidgetItem * item_ = nullptr;
        int iC_init = 0;
        for(int iC=0; iC<columnCount(); iC++)
        {
            if(!neverEditableColumns.contains(iC))
            {
                iC_init = iC;
                break;
            }
        }
        //setCurrentItem(item_);
        setCurrentIndex(model()->index(0, iC_init));
        if(item(0,iC_init)!= nullptr)
            editItem(item(0,iC_init));
        else
            cellWidget(0,iC_init)->setFocus();
    }
    highlightingLine(0);
}

void TableWidgetBase::mouseReleaseEvent(QMouseEvent *event)
{
    auto y = event->pos().y();
    int row = rowAt(y);
    highlightingLine(row);
    QTableWidget::mouseReleaseEvent(event);
}

void TableWidgetBase::slotCompletingRow()
{
    completingRow(true);
}

bool TableWidgetBase::completingRow(bool clickButton)
{
    auto curRow = this->currentRow();
    auto bAllData = true;
    auto bLocked = true;
    for(int iCol=0; iCol<columnCount(); iCol++)
    {
        if(item(curRow, iCol))
        {
            if(!item(curRow, iCol)->flags().testFlag(Qt::ItemIsEditable))
            {
                continue;
            }
            bLocked = false;
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
            if(cellWidget(curRow,iCol)->property("locked").toBool())
            {
                continue;
            }
            bLocked = false;
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
    if(bAllData && !bLocked)
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

void TableWidgetBase::slotFocusInPassword(QWidget *wgt)
{
    int curColumn = -1;
    int curRow = -1;
    auto focus_wgt = qApp->focusWidget();

    for(int iR=0; iR<rowCount(); iR++)
    {
        for(int iC=0; iC<columnCount(); iC++)
        {
            if(cellWidget(iR, iC) == focus_wgt
                    ||
               cellWidget(iR, iC) == focus_wgt->parentWidget())
            {
                curColumn = iC;
                curRow = iR;
                break;
            }
        }
    }
    setCurrentCell(curRow, curColumn);
}

void TableWidgetBase::slotClickedPasswordChild()
{
    auto wgt = qobject_cast<QWidget*>(sender());
    for (int iR=0; iR<rowCount(); iR++)
    {
        for(int iC=0; iC<columnCount(); iC++)
        {
            if(cellWidget(iR,iC) == wgt)
            {
                highlightingLine(iR);
                break;
            }
        }
    }
}

void TableWidgetBase::slotFinishEditing()
{
    auto curColumn = this->currentColumn();
    auto curRow = this->currentRow();
    if(bNewCol)
        curColumn--;
    bool bCompleteRow = false;
    if(curRow == 0)
    {
        bCompleteRow = completingRow();
    }
    if(!bCompleteRow)
    {
        int newColumn = curColumn+1;
        if(newColumn >= columnCount())
        {
            for(int iC=0; iC<columnCount(); iC++)
            {
                if(!neverEditableColumns.contains(iC))
                {
                    newColumn = iC;
                    break;
                }
            }
        }
        if(!bNewCol)
        {
            bNewCol = true;
            if(cellWidget(curRow, newColumn))
                cellWidget(curRow, newColumn)->setFocus();
        }
        setCurrentIndex(model()->index(curRow, newColumn));
        bNewCol= false;
    }
}
