#include "global.h"
#include <QWidget>
#include <QApplication>
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
