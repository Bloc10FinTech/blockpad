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
    Q_PROPERTY(bool locked MEMBER m_locked WRITE setLocked)
    Q_PROPERTY(bool highlighted MEMBER m_highlighted WRITE setHighlighted)
public:
    explicit PasswordWidget(QWidget *parent = 0);
    ~PasswordWidget();
    void setText(QString text);
    QString text();
    void setLocked(bool bLock);
    void setHighlighted(bool bOn);
protected:
    void focusInEvent(QFocusEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private:
    Ui::PasswordWidget *ui;
    QPalette paletteEditable;
    QString _text;
    bool m_highlighted {false};
    bool m_locked {false};
public slots:
    void slotAllwaysVisible(bool allways);
private slots:
    void slotVisibleClicked(bool bCheck);
    void slotTextEdited(QString str);
signals:
    void enterLineEditPressed();
    void focusIn(QWidget *wgt);
    void clickedToChild();
};

#endif // PASSWORDWIDGET_H
