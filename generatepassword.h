#ifndef GENERATEPASSWORD_H
#define GENERATEPASSWORD_H

#include <QWidget>
#include <QString>
#include "global.h"

namespace Ui {
class GeneratePassword;
}

class GeneratePassword : public QWidget
{
    Q_OBJECT

public:
    explicit GeneratePassword(QWidget *parent = 0);
    ~GeneratePassword();

private:
    Ui::GeneratePassword *ui;
    QString symbols {"@#$%*&"};
    QString numbers {"23456789"};
    QString lowercases {"abcdefghjkmnpqrstuvwxyz"};
    QString uppercases {"ABCDEFGHJKMNPQRSTUVWXYZ"};
    QString similars {"il1ILo0O"};
    QString ambiguouses {"{}[]()/'\"\\`~,;:.!?<>^+-=_"};
private slots:
    void slotGeneratePassword();
    void slotCheckBoxesChanged();
};

#endif // GENERATEPASSWORD_H
