#ifndef NETWORKLICENSESERVER_H
#define NETWORKLICENSESERVER_H

#include <QObject>
#include <QSettings>
#include <QNetworkReply>
class QNetworkAccessManager;

class NetworkLicenseServer : public QObject
{
    Q_OBJECT
public:
    NetworkLicenseServer(QObject *parent=Q_NULLPTR);
    void sendActivateRequest(QString license, QString devName);
    void sendCheckRequest(QString license);
private:
    QNetworkAccessManager * nam;
    QSettings settings;
    void activateFinished(QNetworkReply *reply);
    void checkFinished(QNetworkReply *reply);
private slots:
    void slotReplyFinished(QNetworkReply *reply);
signals:
    void sigNetworkError(QNetworkReply::NetworkError);
    void sigActivateFinished(bool bSuccess,
                             QString id,
                             QString strError);
    void sigCheckFinished(bool bSuccess,
                             QString strError);
};

#endif // NETWORKLICENSESERVER_H
