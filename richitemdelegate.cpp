#include "richitemdelegate.h"
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QTextDocument>
#include <QDebug>
#include <QFontMetrics>
#include <QApplication>
#include <QKeyEvent>

bool bSetModelData {false};
RichItemDelegate::RichItemDelegate(QObject *parent):QItemDelegate(parent)
{
}

void RichItemDelegate::setModelData(QWidget *editor,
                  QAbstractItemModel *model,
                  const QModelIndex &index) const
{
    bSetModelData = true;
    QItemDelegate::setModelData(editor, model, index);
    auto displayData = model->data(index, Qt::DisplayRole).toString();
    QFontMetrics fm(qApp->font());
    model->setData(index, fm.width(displayData), textWidthRole);
    if(!displayData.isEmpty())
        displayData = "<a href=\"" + displayData
                            +"\">" + displayData +"</a>";
    model->setData(index, displayData, Qt::DisplayRole);
    model->setData(index, displayData, Qt::EditRole);
    bSetModelData = false;
}

QString RichItemDelegate::nameWebSite(QString hyperlink)
{
    QString name= hyperlink;
    auto indexBegin = name.indexOf("\">");
    name = name.mid(indexBegin).remove("\">").remove("</a>");
    return name;
}

bool RichItemDelegate::eventFilter(QObject *editor, QEvent *event)
{
    if(event->type() ==  QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Tab
                ||
            keyEvent->key() == Qt::Key_Return
                ||
            keyEvent->key() == Qt::Key_Enter)
        {
            emit sigTabEnterInput();
            return true;
        }
    }
    return QItemDelegate::eventFilter(editor, event);
}

void RichItemDelegate::setEditorData(QWidget *editor,
                   const QModelIndex &index) const
{
    if(!bSetModelData)
    {
        auto editData = index.model()->data(index, Qt::EditRole).toString();
        editData = nameWebSite(editData);
        QAbstractItemModel * model = const_cast<QAbstractItemModel *>(index.model());
        model->setData(index, editData, Qt::EditRole);
        model->setData(index, editData, Qt::DisplayRole);
        QItemDelegate::setEditorData(editor, index);
    }
}

void RichItemDelegate::drawDisplay(QPainter *painter,
                 const QStyleOptionViewItem &option,
                 const QRect &rect, const QString &text) const
{
    if( option.state & QStyle::State_Selected )
        painter->fillRect( rect, option.palette.highlight() );
    painter->save();

    QTextDocument document;
    document.setTextWidth(rect.width());
    if (!text.isEmpty())
    {
        document.setHtml(text);
        painter->translate(rect.topLeft());
        document.drawContents(painter);
    }

    painter->restore();
}
