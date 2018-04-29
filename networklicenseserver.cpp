#include "networklicenseserver.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "global.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>
#include <QMessageAuthenticationCode>
#include <QCryptographicHash>

#define defLicenseWebSite "https://dev.fxbot.market/blockpad/blockpad.php"

NetworkLicenseServer::NetworkLicenseServer(QObject *parent):
    QObject(parent)
{
    nam = new QNetworkAccessManager(this);

    //signals-slots connects
    {
        connect(nam, &QNetworkAccessManager::finished,
                this, &NetworkLicenseServer::slotReplyFinished);
    }
}

void NetworkLicenseServer::sendActivateRequest(QString license, QString devName)
{
    QByteArray data;
    //fill data
    {
        QJsonObject object;
        object["cmd"] = "Activate";
        object["license_key"] = license;
        object["mac_address"] = Utilities::macAddress() + settings.value("device_name").toString();
        object["device_name"] = devName;
        //fill hash
        {
            QByteArray hashData;
            hashData.append(object["license_key"].toString());
            hashData.append(object["mac_address"].toString());
            hashData.append(object["device_name"].toString());
            object["hash"] = QString(QCryptographicHash::hash(
                        hashData,QCryptographicHash::Sha256).toHex());
        }
        QJsonDocument doc(object);
        data = doc.toJson();
    }
    QNetworkRequest request(QUrl(defLicenseWebSite));
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/json");
    auto reply = nam->post(request, data);
    reply->setProperty("type", "Activate");
}

void NetworkLicenseServer::sendCheckRequest(QString license)
{
    QByteArray data;
    //fill data
    {
        QJsonObject object;
        object["cmd"] = "Check";
        object["id"] = qApp->property(defIdProperty).toString();
        object["license_key"] = license;
        object["mac_address"] = Utilities::macAddress() + settings.value("device_name").toString();
        //fill hash
        {
            QByteArray hashData;
            hashData.append(object["id"].toString());
            hashData.append(object["license_key"].toString());
            hashData.append(object["mac_address"].toString());
            qDebug() << "hashData: " << hashData;
            object["hash"] = QString(QCryptographicHash::hash(
                        hashData,QCryptographicHash::Sha256).toHex());
        }
        QJsonDocument doc(object);
        data = doc.toJson();
    }
    QNetworkRequest request(QUrl(defLicenseWebSite));
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/json");
    auto reply = nam->post(request, data);
    reply->setProperty("type", "Check");
}

void NetworkLicenseServer::activateFinished(QNetworkReply *reply)
{
    auto data = reply->readAll();
    QJsonDocument document = QJsonDocument::fromJson(data);
    auto object = document.object();
    bool bSuccess = object.value("success").toBool();
    QString strError = object.value("error").toString();
    QString id = QString::number(object.value("id").toInt());
    if(bSuccess)
        qApp->setProperty(defIdProperty, id);
    emit sigActivateFinished(bSuccess, id, strError);
}

void NetworkLicenseServer::checkFinished(QNetworkReply *reply)
{
    auto data = reply->readAll();
    QJsonDocument document = QJsonDocument::fromJson(data);
    auto object = document.object();
    bool bSuccess = object.value("success").toBool();
    QString strError = object.value("error").toString();
    emit sigCheckFinished(bSuccess, strError);
}

void NetworkLicenseServer::slotReplyFinished(QNetworkReply *reply)
{
    auto error = reply->error();
    if(error != QNetworkReply::NoError)
    {
        emit sigNetworkError(error);
        return;
    }

    auto typeReply = reply->property("type").toString();
    if(typeReply == "Activate")
    {
        activateFinished(reply);
    }
    if(typeReply == "Check")
    {
        checkFinished(reply);
    }
}
