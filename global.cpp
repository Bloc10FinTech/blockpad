#include "global.h"
#include <QWidget>
#include <QApplication>
#include <QStandardPaths>
#include <QCoreApplication>
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

QString Utilities::appFilesDirectory()
{
#if defined(WIN32) || defined(WIN64)
    return QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#endif
#ifdef __APPLE__
    return QCoreApplication::applicationDirPath();
#endif
}
