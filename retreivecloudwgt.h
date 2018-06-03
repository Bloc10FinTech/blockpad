#ifndef RETREIVECLOUDWGT_H
#define RETREIVECLOUDWGT_H

#include <QWidget>
#include "networklicenseserver.h"
#include <QMap>
#include <QSettings>

namespace Ui {
class RetreiveCloudWgt;
}

class RetreiveCloudWgt : public QWidget
{
    Q_OBJECT

public:
    explicit RetreiveCloudWgt(QWidget *parent = 0);
    ~RetreiveCloudWgt();
private:
    Ui::RetreiveCloudWgt *ui;
    NetworkLicenseServer netwLicenseServer;
    QSettings settings;
    enum  columnsRetreiveCloud {LicenseKey, devName, id, download,
                               nColumns};
private slots:
    void slotLoginClicked();
    void slotDownloadEditFinished();
    void slotAutoOpenChanged(bool bCheck);
    void slotDownloadClicked();
    void slotBlockPadsFinished(bool bSuccess,
                              //QMap<license, QMap<device {id0, id1,...}>>
                               QMap<QString, QMap<QString, QStringList>> cloudBlockPads);
};

#endif // RETREIVECLOUDWGT_H
