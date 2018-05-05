#include "passwordwidget.h"
#include "ui_passwordwidget.h"
#include <QPalette>
#include <QDebug>
#include <QKeyEvent>
#include "global.h"
#include <QStyleFactory>

PasswordWidget::PasswordWidget(QWidget *parent, bool noPassword) :
    QFrame(parent),
    ui(new Ui::PasswordWidget),
    noPassword(noPassword)
{
    ui->setupUi(this);
    QPixmap pixmap("://Icons/locked.png");
    pixmap = pixmap.scaled(15, 15);
    ui->labelIcon->setPixmap(pixmap);
    setLocked(false);
#ifdef __APPLE__
    setStyle(QStyleFactory::create("Windows"));
#endif
    //diseble = enable palettes
    {
        auto pal = palette();
        for(int iPal=0; iPal<QPalette::NColorRoles; iPal++)
        {
            paletteEditable.setColor(QPalette::All,
                                     (QPalette::ColorRole)iPal,
                                     pal.color(QPalette::WindowText));
        }
        auto widgets = findChildren<QWidget *>();
        foreach(auto & wgt, widgets)
        {
            wgt->setPalette(paletteEditable);
        }
        setPalette(paletteEditable);
    }
    //signals/slots connects
    {
        connect(ui->checkBoxVisible, &QCheckBox::toggled,
                this, &PasswordWidget::slotVisibleClicked);
        connect(ui->lineEditPassword, &QLineEdit::textEdited,
                this, &PasswordWidget::slotTextEdited);
        connect(ui->lineEditPassword, &QLineEdit::returnPressed,
                this, &PasswordWidget::enterLineEditPressed);
    }
    //event filters
    {
        ui->lineEditPassword->installEventFilter(this);
        ui->labelPassword->installEventFilter(this);
        ui->checkBoxVisible->installEventFilter(this);
    }
    if(noPassword)
        ui->checkBoxVisible->hide();

    ui->lineEditPassword->setAttribute(Qt::WA_MacShowFocusRect,0);
}

void PasswordWidget::slotVisibleClicked(bool bCheck)
{
    if(!noPassword)
    {
        if(bCheck)
        {
            ui->lineEditPassword->setText(_text);
            ui->labelPassword->setText(_text);
        }
        else
        {
            QString newStr = _text;
            ui->lineEditPassword->setText(newStr.fill('*'));
            ui->labelPassword->setText(newStr.fill('*'));
        }
    }
}

void PasswordWidget::setHighlighted(bool bOn)
{
    auto widgets = findChildren<QWidget *>();
    foreach(auto wgt, widgets)
    {
        wgt->setProperty("highlighted", bOn);
        wgt->style()->unpolish(wgt);
        wgt->style()->polish(wgt);
    }
    m_highlighted = bOn;
    this->style()->unpolish(this);
    this->style()->polish(this);
}

void PasswordWidget::setLocked(bool bLock)
{
    if(!bLock)
    {
        ui->checkBoxVisible->setChecked(true);
    }
    ui->lineEditPassword->setEnabled(!bLock);
    ui->lineEditPassword->setVisible(!bLock);
    ui->labelPassword->setVisible(bLock);
    if(!bLock)
    {
        ui->labelIcon->hide();
        ui->checkBoxVisible->setProperty("locked", false);
        setToolTip("");
    }
    else
    {
        ui->labelIcon->show();
        ui->checkBoxVisible->setProperty("locked", true);
        setToolTip("to make editable item click it and input Ctrl+Y");
    }
    m_locked = bLock;
    ui->checkBoxVisible->style()->unpolish(ui->checkBoxVisible);
    ui->checkBoxVisible->style()->polish(ui->checkBoxVisible);
    this->style()->unpolish(this);
    this->style()->polish(this);
}

void PasswordWidget::slotTextEdited(QString str)
{
    _text = str;
    ui->labelPassword->setText(_text);
}

void PasswordWidget::focusInEvent(QFocusEvent *event)
{
    ui->lineEditPassword->setFocus();
    //emit focusIn(this);
}

void PasswordWidget::setText(QString text)
{
    ui->lineEditPassword->setText(text);
    ui->labelPassword->setText(text);
    _text = text;
}

QString PasswordWidget::text()
{
    return _text;
}

bool PasswordWidget::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonRelease)
    {
        emit clickedToChild();
    }
    if(obj == ui->lineEditPassword)
    {
        if(event->type() == QEvent::FocusIn)
        {
            ui->checkBoxVisible->setChecked(true);
            setFrameShape(QFrame::Box);
            emit focusIn(this);
        }
        if(event->type() == QEvent::FocusOut)
        {
            if(!ui->checkBoxVisible->hasFocus())
                setFrameShape(QFrame::NoFrame);
        }
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Tab)
            {
                emit enterLineEditPressed();
                return true;
            }
        }
    }
    if(obj == ui->checkBoxVisible)
    {
        if(event->type() == QEvent::FocusOut)
        {
            if(!ui->lineEditPassword->hasFocus())
                setFrameShape(QFrame::NoFrame);
        }
    }
    return QWidget::eventFilter(obj, event);
}

void PasswordWidget::slotAllwaysVisible(bool allways)
{
    if(!noPassword)
    {
        if(allways)
        {
            slotVisibleClicked(true);
            ui->checkBoxVisible->hide();
        }
        else
        {
            ui->checkBoxVisible->show();
            ui->checkBoxVisible->setChecked(false);
            slotVisibleClicked(false);
        }
    }
}

PasswordWidget::~PasswordWidget()
{
    delete ui;
}
