#include "activatewgt.h"
#include "ui_activatewgt.h"
#include <QMessageBox>
#include "global.h"
ActivateWgt::ActivateWgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ActivateWgt)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    //ui->pushButtonActivate->setEnabled(false);
    auto devName = settings.value("device_name").toString();
    if(!devName.isEmpty())
    {
        ui->lineEditDeviceName->setText(devName);
        //ui->lineEditDeviceName->setReadOnly(true);
        ui->lineEditLicenseKey->setFocus();
    }
    else
    {
        ui->lineEditDeviceName->setFocus();
        ui->lineEditLicenseKey->setReadOnly(true);
    }
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
}

void ActivateWgt::slotIdFinished()
{
    qApp->setProperty(defIdProperty, ui->lineEditId->text());
}

void ActivateWgt::slotActivateClicked()
{
    netwLicenseServer.sendActivateRequest(ui->lineEditLicenseKey->text());
}

void ActivateWgt::slotNetworkError(QNetworkReply::NetworkError err)
{
    QMessageBox::information(this, "Activate",
                             "err = " + err);
}

void ActivateWgt::slotActivateFinished(bool bSuccess,
                                       QString id,
                                       QString strError)
{
    QMessageBox::information(this, "Activate",
                             "bSuccess = " + QString::number(bSuccess)
                             + " id= " + id + " strError = " + strError);
}

void ActivateWgt::slotDeviceNameFinished()
{
    if(!ui->lineEditDeviceName->text().isEmpty())
    {
        ui->lineEditLicenseKey->setReadOnly(false);
        ui->lineEditLicenseKey->setFocus();
        settings.setValue("device_name", ui->lineEditDeviceName->text());
        settings.sync();
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
//    if(!ui->lineEditLicenseKey->text().isEmpty())
//    {
        //ui->pushButtonActivate->setEnabled(true);
//    }
}

ActivateWgt::~ActivateWgt()
{
    delete ui;
}
