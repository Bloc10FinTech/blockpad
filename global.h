#ifndef GLOBAL_H
#define GLOBAL_H
#include <QString>

class QWidget;

#define defVersionDB "0.4"

#if defined(WIN32) || defined(WIN64)
#define appFontPointSize 10
#endif
#ifdef __APPLE__
#define appFontPointSize 13
#endif
#define appFontWeight QFont::Normal
#define defColorNoEditable "#f8f8f8"
#define defHashKey "cmpsv"
#define defPathBlockpads "blockpads"
#define defExternalBlockpads "external_blockpads"

//settings
#define defCurrentEmail "CurrentEmail"

//properties qapplication
#define defEmailProperty "email"
#define defPasswordProperty "password"
#define defFileProperty "fileBlockpad"
#define def2FA_Code "2FA_Code"

namespace Utilities {
    void setAppFamilyFont(  QWidget * wgt,
                            int pointSize,
                            int weight = -1,
                            bool italic = false);
    QString filesDirectory();
    QString applicationPath();
}
#endif // GLOBAL_H
