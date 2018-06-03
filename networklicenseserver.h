#ifndef NETWORKLICENSESERVER_H
#define NETWORKLICENSESERVER_H

#include <QObject>
#include <QSettings>
#include <QNetworkReply>
#include <QMap>
class QNetworkAccessManager;

class NetworkLicenseServer : public QObject
{
    Q_OBJECT
public:
    NetworkLicenseServer(QObject *parent=Q_NULLPTR);
    void sendActivateRequest(QString license, QString devName);
    void sendCheckRequest(QString license);
    void sendCloudBlockPadsRequest(QString username, QString password);
private:
    QNetworkAccessManager * nam;
    QSettings settings;
    void activateFinished(QNetworkReply *reply);
    void checkFinished(QNetworkReply *reply);
    void cloudBlockPadsFinished(QNetworkReply *reply);
private slots:
    void slotReplyFinished(QNetworkReply *reply);
signals:
    void sigNetworkError(QNetworkReply::NetworkError);
    void sigActivateFinished(bool bSuccess,
                             QString id,
                             QString strError);
    void sigCheckFinished(bool bSuccess,
                             QString strError);
    void sigBlockPadsFinished(bool bSuccess,
                              //QMap<license, QMap<device {id0, id1,...}>>
                              QMap<QString, QMap<QString, QStringList>> cloudBlockPads);
};

#endif // NETWORKLICENSESERVER_H
