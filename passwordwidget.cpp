#include "passwordwidget.h"
#include "ui_passwordwidget.h"
#include <QPalette>
#include <QDebug>
#include "global.h"

PasswordWidget::PasswordWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::PasswordWidget)
{
    ui->setupUi(this);
    QPixmap pixmap("://Icons/locked.png");
    pixmap = pixmap.scaled(15, 15);
    ui->labelIcon->setPixmap(pixmap);
    setLocked(false);
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
    }
}

void PasswordWidget::slotVisibleClicked(bool bCheck)
{
    if(bCheck)
        ui->lineEditPassword->setText(_text);
    else
    {
        QString newStr = _text;
        ui->lineEditPassword->setText(newStr.fill('*'));
    }
}

void PasswordWidget::setLocked(bool bLock)
{
    if(!bLock)
    {
        ui->checkBoxVisible->setChecked(true);
    }
    ui->lineEditPassword->setEnabled(!bLock);
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
    ui->checkBoxVisible->style()->unpolish(ui->checkBoxVisible);
    ui->checkBoxVisible->style()->polish(ui->checkBoxVisible);
}

void PasswordWidget::slotTextEdited(QString str)
{
    _text = str;
}

void PasswordWidget::focusInEvent(QFocusEvent *event)
{
    ui->lineEditPassword->setFocus();
    emit focusIn(this);
}

void PasswordWidget::setText(QString text)
{
    ui->lineEditPassword->setText(text);
    _text = text;
}

QString PasswordWidget::text()
{
    return _text;
}

bool PasswordWidget::eventFilter(QObject *obj, QEvent *event)
{
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
            setFrameShape(QFrame::NoFrame);
        }
    }
    return QWidget::eventFilter(obj, event);
}

void PasswordWidget::slotAllwaysVisible(bool allways)
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

PasswordWidget::~PasswordWidget()
{
    delete ui;
}
