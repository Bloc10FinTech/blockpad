#ifndef ACTIVATEWGT_H
#define ACTIVATEWGT_H

#include <QWidget>
#include <QSettings>
#include "networklicenseserver.h"

namespace Ui {
class ActivateWgt;
}

class ActivateWgt : public QWidget
{
    Q_OBJECT

public:
    explicit ActivateWgt(QWidget *parent = 0);
    ~ActivateWgt();

private:
    Ui::ActivateWgt *ui;
    QSettings settings;
    NetworkLicenseServer netwLicenseServer;
private slots:
    void slotDeviceNameFinished();
    void slotLicenseKeyFinished();
    void slotIdFinished();
    void slotActivateClicked();
    void slotCheckClicked();
    void slotNetworkError(QNetworkReply::NetworkError);
    void slotActivateFinished(bool bSuccess,
                              QString id,
                              QString strError);
    void slotCheckFinished(bool bSuccess,
                           QString strError);
};

#endif // ACTIVATEWGT_H
