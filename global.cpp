#include "global.h"
#include <QWidget>
#include <QApplication>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QNetworkInterface>

#define defNetworkAdapterName "ethernet"

void Utilities::setAppFamilyFont(   QWidget * wgt,
                                    int pointSize,
                                    int weight,
                                    bool italic)
{
    auto font = qApp->font();
    font.setPointSize(pointSize);
    font.setWeight(weight);
    font.setItalic(italic);
    wgt->setFont(font);
}

QString Utilities::filesDirectory()
{
    return QStandardPaths::writableLocation(QStandardPaths::DataLocation);
}

QString Utilities::applicationPath()
{
#ifdef __APPLE__
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    return dir.path();
#else
    return QCoreApplication::applicationDirPath();
#endif
}

QString Utilities::macAddress()
{
    QString macAddr;
    auto interfaces = QNetworkInterface::allInterfaces();
    foreach(auto inface, interfaces)
    {
        if (inface.humanReadableName().toLower().contains(defNetworkAdapterName)
                &&
            inface.name().toLower().contains(defNetworkAdapterName))
        {
            macAddr = inface.hardwareAddress();
            return macAddr;
        }
    }
    foreach(auto inface, interfaces)
    {
        if (inface.name().toLower().contains(defNetworkAdapterName))
        {
            macAddr = inface.hardwareAddress();
            return macAddr;
        }
    }
    macAddr = interfaces[0].hardwareAddress();
    return macAddr;
}
