#ifndef GLOBAL_H
#define GLOBAL_H
#include <QString>

class QWidget;

#define defVersionDB "0.5"

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
