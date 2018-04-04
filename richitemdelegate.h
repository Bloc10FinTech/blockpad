#ifndef RICHITEMDELEGATE_H
#define RICHITEMDELEGATE_H

#include <QObject>
#include <QItemDelegate>
class RichItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    RichItemDelegate(QObject *parent = 0);
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const;
    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const;
    static const Qt::ItemDataRole textWidthRole {(Qt::ItemDataRole)(Qt::UserRole)};
    static QString nameWebSite(QString hyperlink);
protected:
    void drawDisplay(QPainter *painter,
                     const QStyleOptionViewItem &option,
                     const QRect &rect, const QString &text) const;
    bool eventFilter(QObject *editor, QEvent *event);
private:

};

#endif // RICHITEMDELEGATE_H
