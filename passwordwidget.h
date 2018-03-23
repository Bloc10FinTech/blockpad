#ifndef PASSWORDWIDGET_H
#define PASSWORDWIDGET_H

#include <QFrame>

namespace Ui {
class PasswordWidget;
}

class PasswordWidget : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(bool locked WRITE setLocked)
public:
    explicit PasswordWidget(QWidget *parent = 0);
    ~PasswordWidget();
    void setText(QString text);
    QString text();
    void setLocked(bool bLock);
protected:
    void focusInEvent(QFocusEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
private:
    Ui::PasswordWidget *ui;
    QPalette paletteEditable;
    QString _text;
public slots:
    void slotAllwaysVisible(bool allways);
private slots:
    void slotVisibleClicked(bool bCheck);
    void slotTextEdited(QString str);
signals:
    void enterLineEditPressed();
    void focusIn(QWidget *wgt);
};

#endif // PASSWORDWIDGET_H
