#include "global.h"
#include <QWidget>
#include <QApplication>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
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
#if defined(WIN32) || defined(WIN64)
    return QCoreApplication::applicationDirPath();
#endif
#ifdef __APPLE__
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    qDebug() << "dir.path(): " << dir.path();
    return dir.path();
#endif
}
