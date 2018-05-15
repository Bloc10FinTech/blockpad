#include "searchwgt.h"
#include "ui_searchwgt.h"
#include <QEvent>


SearchWgt::SearchWgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchWgt)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlag(Qt::Window);
    //signals-slots connects
    {
        //transparency
        connect(ui->groupBoxTransparency, &QGroupBox::clicked,
                this, &SearchWgt::slotTransparencyGroupClicked);
        connect(ui->horizontalSliderTransparency, &QSlider::valueChanged,
                this, &SearchWgt::slotSliderTransparencyChanged);
        connect(ui->radioButtonLosingFocus, &QRadioButton::clicked,
                this, &SearchWgt::slotOnLosingFocusClicked);
        connect(ui->radioButtonAllways, &QRadioButton::clicked,
                this, &SearchWgt::slotAllwaysClicked);

       //find
        connect(ui->pushButtonFindAllCurrent, &QPushButton::clicked,
                this, &SearchWgt::slotFindAllCurrent);
    }
    ui->comboBoxFindWhat->setFocus();
}

bool SearchWgt::event(QEvent *e)
{
    // window was activated
    if (e->type() == QEvent::WindowActivate)
    {
        if(ui->radioButtonLosingFocus->isChecked()
                ||
           !ui->groupBoxTransparency->isChecked())
        {
            setWindowOpacity(1.0);
        }
        else
        {
            if(ui->horizontalSliderTransparency->value()
                    <10)
                setWindowOpacity(0.1);
        }
    }
    // window was deactivated
    if (e->type() == QEvent::WindowDeactivate)
    {
        if(ui->radioButtonLosingFocus->isChecked())
        {
            setWindowOpacity(0.01 *
                             (qreal)ui->horizontalSliderTransparency->value());
        }
    }
    return QWidget::event(e);
}

void SearchWgt::slotFindAllCurrent()
{
    if(!ui->comboBoxFindWhat->currentText().isEmpty())
    {
        emit sigFindAllCurrentFile(ui->comboBoxFindWhat->currentText(),
                                   ui->checkBoxRegularExpressions->isChecked(),
                                   ui->checkBoxWholeWord->isChecked(),
                                   ui->checkBoxCase->isChecked());
        ui->comboBoxFindWhat->addItem(ui->comboBoxFindWhat->currentText());
    }
}
void SearchWgt::slotOnLosingFocusClicked()
{
    setWindowOpacity(1.0);
}

void SearchWgt::slotAllwaysClicked()
{
    setWindowOpacity(0.01 *
                     (qreal)ui->horizontalSliderTransparency->value());
}

void SearchWgt::slotSliderTransparencyChanged(int value)
{
    if(ui->radioButtonAllways->isChecked()
            &&
       ui->groupBoxTransparency->isChecked())
    {
        setWindowOpacity(0.01 * (qreal)value);
    }
}

void SearchWgt::slotTransparencyGroupClicked(bool checked)
{
    if(checked)
    {
        if(ui->radioButtonAllways->isChecked())
        {
            setWindowOpacity(0.01 *
                             (qreal)ui->horizontalSliderTransparency->value());
        }
    }
    else
    {
        setWindowOpacity(1.0);
    }
}

SearchWgt::~SearchWgt()
{
    delete ui;
}
