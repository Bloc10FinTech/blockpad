#include "activatewgt.h"
#include "ui_activatewgt.h"
#include <QMessageBox>
#include <QFontMetrics>
#include <QDateTime>
#include "global.h"
ActivateWgt::ActivateWgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ActivateWgt)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    Utilities::setAppFamilyFont(ui->labelTitle, 16,QFont::Bold);
    Utilities::setAppFamilyFont(ui->labelStatus, qApp->font().pointSize()+1,QFont::Bold);
    Utilities::setAppFamilyFont(ui->labelDescriptionDevName, qApp->font().pointSize(),QFont::Normal, true);
    Utilities::setAppFamilyFont(ui->labelDescriptionLicenseKey, qApp->font().pointSize(),QFont::Normal, true);
    auto devName = settings.value("device_name").toString();
    if(!devName.isEmpty())
    {
        ui->lineEditDeviceName->setText(devName);
        ui->lineEditDeviceName->setReadOnly(true);
        ui->lineEditLicenseKey->setFocus();
    }
    else
    {
        ui->lineEditDeviceName->setFocus();
    }
    bool isActive = qApp->property(defLicenseIsActive).toBool();
    if(isActive)
    {
        ui->labelIcon->setPixmap(QPixmap("://Icons/licenseActivate.png"));
        ui->pushButtonActivate->setEnabled(false);
        ui->labelStatus->setProperty("activated", true);
        ui->labelStatus->setText("License successfully activated!");
        ui->lineEditLicenseKey->setReadOnly(true);
    }
    else
    {
        ui->labelIcon->setPixmap(QPixmap("://Icons/licenseNoActivate.png"));
        ui->pushButtonActivate->setEnabled(true);
        ui->labelStatus->setProperty("activated", false);
        ui->labelStatus->setText("License not activated");
    }
    ui->lineEditLicenseKey->setText(qApp->property(defLicenseProperty).toString());
    ui->labelDescriptionDevName->setText("Device name is used to manage your devices in checkout page. Be carefull with device name choise! Device name must be unical among all other your device names. After you was activated license in your device you can not change device name. You can allways see your device name here");
    ui->labelDescriptionLicenseKey->setText("To get license key please click \"Premium version\". You can allways see your license key in this page.");

    int widthMin = QFontMetrics(qApp->font()).width("WWWWWW-WWWWWW-WWWWWW-WWWWWW") +10;
    ui->lineEditDeviceName->setMinimumWidth(widthMin);
    ui->lineEditLicenseKey->setMinimumWidth(widthMin);
    //signals/slots connects
    {
        connect(ui->lineEditDeviceName, &QLineEdit::editingFinished,
                this, &ActivateWgt::slotDeviceNameFinished);
        connect(ui->lineEditLicenseKey, &QLineEdit::editingFinished,
                this, &ActivateWgt::slotLicenseKeyFinished);
        connect(ui->lineEditId, &QLineEdit::editingFinished,
                this, &ActivateWgt::slotIdFinished);
        connect(&netwLicenseServer, &NetworkLicenseServer::sigActivateFinished,
                this, &ActivateWgt::slotActivateFinished);
        connect(&netwLicenseServer, &NetworkLicenseServer::sigCheckFinished,
                this, &ActivateWgt::slotCheckFinished);
        connect(&netwLicenseServer, &NetworkLicenseServer::sigNetworkError,
                this, &ActivateWgt::slotNetworkError);
        connect(ui->pushButtonActivate, &QPushButton::clicked,
                this, &ActivateWgt::slotActivateClicked);
        connect(ui->pushButtonCheck, &QPushButton::clicked,
                this, &ActivateWgt::slotCheckClicked);
    }
#ifndef TEST_LICENSE
    ui->widgetCheck->hide();
    ui->widgetId->hide();
#endif
    adjustSize();
}

void ActivateWgt::slotIdFinished()
{
    qApp->setProperty(defIdProperty, ui->lineEditId->text());
}

void ActivateWgt::slotActivateClicked()
{
    if(ui->lineEditDeviceName->text().isEmpty())
    {
        QMessageBox::critical(this, windowTitle(),
                              "Please input device name!");
        return;
    }

    if(ui->lineEditLicenseKey->text().isEmpty())
    {
        QMessageBox::critical(this, windowTitle(),
                              "Please input license key!");
        return;
    }

    netwLicenseServer.sendActivateRequest(ui->lineEditLicenseKey->text(),
                                          ui->lineEditDeviceName->text());
    ui->pushButtonActivate->setEnabled(false);
}

void ActivateWgt::slotNetworkError(QNetworkReply::NetworkError err)
{
    QMessageBox::information(this, "License Activation",
                             "Network error = " + QString::number(err));
}

void ActivateWgt::slotActivateFinished(bool bSuccess,
                                       QString id,
                                       QString strError)
{
    qDebug() << "Activated: strError - " + strError;
    if(bSuccess)
    {
        QMessageBox::information(this, windowTitle(),
                                 "License has been successfully activated!");
        ui->lineEditLicenseKey->setReadOnly(true);
        ui->labelStatus->setProperty("activated", true);
        ui->labelStatus->setText("License successfully activated!");
        ui->labelStatus->style()->unpolish(ui->labelStatus);
        ui->labelStatus->style()->polish(ui->labelStatus);
        //change icon
        {
            ui->labelIcon->setPixmap(QPixmap("://Icons/licenseActivate.png"));
            ui->pushButtonActivate->setEnabled(false);
            ui->labelStatus->setProperty("activated", true);
            ui->labelStatus->setText("License successfully activated!");
            ui->lineEditLicenseKey->setReadOnly(true);
        }
        //remembered data
        {
            settings.setValue("device_name", ui->lineEditDeviceName->text());
            settings.sync();
            qApp->setProperty(defLicenseIsActive, true);
            qApp->setProperty(defLicenseProperty, ui->lineEditLicenseKey->text());
            qApp->setProperty(defIdProperty, id);
            qApp->setProperty(defDateStart, QDateTime::currentDateTime().toSecsSinceEpoch());
        }
        emit sigSuccessActivated();
    }
    else
        QMessageBox::critical(this, windowTitle(),
                                 "License could not be activated!");
    ui->pushButtonActivate->setEnabled(true);
}

void ActivateWgt::slotDeviceNameFinished()
{
    if(!ui->lineEditDeviceName->text().isEmpty())
    {
        ui->lineEditLicenseKey->setFocus();
    }
}

void ActivateWgt::slotCheckClicked()
{
    netwLicenseServer.sendCheckRequest(ui->lineEditLicenseKey->text());
}

void ActivateWgt::slotCheckFinished(bool bSuccess,
                       QString strError)
{
    QMessageBox::information(this, "Check",
                             "bSuccess = " + QString::number(bSuccess)
                             + " strError = " + strError);
}

void ActivateWgt::slotLicenseKeyFinished()
{
    auto text = ui->lineEditLicenseKey->text();
    ui->lineEditLicenseKey->setText(text.trimmed());
}

ActivateWgt::~ActivateWgt()
{
    delete ui;
}
