#include "tablewidgetaccounts.h"
#include <QHeaderView>
#include "passwordwidget.h"
#include <QApplication>

TableWidgetAccounts::TableWidgetAccounts(QWidget *parent):
    TableWidgetBase(parent)
{
    QStringList list;
    for(int i_Column=0; i_Column<columnsAccount::N_ColumnsAccount;i_Column++)
    {
        switch(i_Column)
        {
            case columnsAccount::WebSite:
            {
                list<< "Web Site";
            }
            break;
            case columnsAccount::Username:
            {
                list<< "Username";
            }
            break;
            case columnsAccount::Password:
            {
                list<< "Password";
            }
            break;
        }
    }
    setColumnCount((int)columnsAccount::N_ColumnsAccount);
    setHorizontalHeaderLabels(list);
    {
        QHeaderView * HeaderView = horizontalHeader();
        HeaderView->setSectionResizeMode(QHeaderView::Stretch);
    }
    addRow();
}

void TableWidgetAccounts::slotFocusInPassword(QWidget *wgt)
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

void TableWidgetAccounts::slotAllwaysChecked(bool allways)
{
    allwaysVisible = allways;
    emit allwaysChecked(allways);
}

TableWidgetAccounts::~TableWidgetAccounts()
{

}

void TableWidgetAccounts::addRow(QStringList initTexts)
{
    insertRow(0);
    for(int i=0; i<columnCount(); i++)
    {
        if(columnsAccount::Password == i)
        {
            PasswordWidget * wgt = new PasswordWidget(this);
            if(!initTexts.isEmpty())
                wgt->setText(initTexts[i]);
            wgt->slotAllwaysVisible(allwaysVisible);
            connect(this, &TableWidgetAccounts::allwaysChecked,
                    wgt, &PasswordWidget::slotAllwaysVisible);
            connect(wgt, &PasswordWidget::enterLineEditPressed,
                    this, &TableWidgetAccounts::slotFinishEditing);
            connect(wgt, &PasswordWidget::focusIn,
                    this, &TableWidgetAccounts::slotFocusInPassword);
            setCellWidget(0, i,wgt);
        }
        else
        {
            QTableWidgetItem * it = new QTableWidgetItem();
            if(!initTexts.isEmpty())
                it->setText(initTexts[i]);
            setItem(0, i,it);
        }
    }
    setCurrentIndex(model()->index(0, 0));
}
