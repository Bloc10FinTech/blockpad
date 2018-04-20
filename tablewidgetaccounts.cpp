#include "tablewidgetaccounts.h"
#include <QHeaderView>
#include "passwordwidget.h"
#include <QApplication>
#include <QLabel>
#include <richitemdelegate.h>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include <QMouseEvent>

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
    auto richDelegate = new RichItemDelegate(this);
    setItemDelegateForColumn(columnsAccount::WebSite, richDelegate);
    connect(richDelegate, &RichItemDelegate::sigTabEnterInput,
            this, &TableWidgetAccounts::slotFinishEditing);
    setMouseTracking(true);
}

void TableWidgetAccounts::mousePressEvent(QMouseEvent *event)
{
    auto x = event->pos().x();
    auto y = event->pos().y();
    int column = columnAt(x);
    int row = rowAt(y);
    if(columnsAccount::WebSite == column && clickHyperLink)
    {
        auto editData = item(row, column)->text();
        QDesktopServices::openUrl(QUrl(RichItemDelegate::nameWebSite(editData)));
    }
    TableWidgetBase::mousePressEvent(event);
}

void TableWidgetAccounts::mouseMoveEvent(QMouseEvent *event)
{
    auto x = event->pos().x();
    auto y = event->pos().y();
    int column = columnAt(x);
    int row = rowAt(y);
    if(column == columnsAccount::WebSite)
    {
        int xItem = x;
        for(int i=0; i<column; i++)
        {
            xItem -= columnWidth(i);
        }
        int widthLink = model()->data(model()->index(row, column),
                                      RichItemDelegate::textWidthRole).toInt();
        int widthIcon = iconSize().width();

        bool bClickable = false;

        if(row == 0
            &&
           xItem < widthLink)
            bClickable = true;
        if(row > 0
            &&
           xItem < widthLink + widthIcon
            &&
           xItem > widthIcon)
            bClickable = true;

        if(bClickable)
        {
            setCursor(QCursor(Qt::PointingHandCursor));
            clickHyperLink = true;
        }
        else
        {
            setCursor(QCursor(Qt::ArrowCursor));
            clickHyperLink = false;
        }
    }
    TableWidgetBase::mouseMoveEvent(event);
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
        if(columnsAccount::Password == i
                ||
           columnsAccount::Username == i)
        {
            PasswordWidget * wgt;
            if(columnsAccount::Username == i)
                wgt = new PasswordWidget(this, true);
            else
                wgt = new PasswordWidget(this);
            connect(this, &TableWidgetAccounts::allwaysChecked,
                    wgt, &PasswordWidget::slotAllwaysVisible);
            connect(wgt, &PasswordWidget::enterLineEditPressed,
                    this, &TableWidgetAccounts::slotFinishEditing);
            connect(wgt, &PasswordWidget::focusIn,
                    this, &TableWidgetAccounts::slotFocusInPassword);
            connect(wgt, &PasswordWidget::clickedToChild,
                    this, &TableWidgetAccounts::slotClickedPasswordChild);
            if(!initTexts.isEmpty())
                wgt->setText(initTexts[i]);
            wgt->slotAllwaysVisible(allwaysVisible);
            setCellWidget(0, i,wgt);
        }
        else
        {
            QTableWidgetItem * it = new QTableWidgetItem();
            setItem(0, i,it);
            if(!initTexts.isEmpty())
            {
                it->setText(initTexts[i]);
                if(columnsAccount::WebSite == i)
                {
                    QFontMetrics fm(qApp->font());
                    model()->setData(model()->index(0,columnsAccount::WebSite),
                                     fm.width(RichItemDelegate::nameWebSite(initTexts[i])),
                                     RichItemDelegate::textWidthRole);
                }
            }
        }
    }
    setCurrentIndex(model()->index(0, 0));
}
