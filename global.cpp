#include "global.h"
#include <QWidget>
#include <QApplication>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDebug>
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
