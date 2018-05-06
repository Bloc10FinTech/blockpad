#ifndef ONETIMEPADGENERATOR_H
#define ONETIMEPADGENERATOR_H

#include <QWidget>
#include <QRandomGenerator>

namespace Ui {
class OneTimePadGenerator;
}

class OneTimePadGenerator : public QWidget
{
    Q_OBJECT

public:
    explicit OneTimePadGenerator(QWidget *parent = 0);
    ~OneTimePadGenerator();

private:
    Ui::OneTimePadGenerator *ui;
    QRandomGenerator randomGenerator;
    QString numbers {"0123456789"};
    QString lowercases {"abcdefghjklmnpqrstuvwxyz"};
    QString uppercases {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
    QString gibberishs  {"@#$%*&{}[]()/'\"\\`~,;:.!?<>^+-=_"};
    QChar defaultSeparator {'-'};
private slots:
    void slotGenerateClicked();
    void slotCompositionChecked(bool checked);
    void slotFormatChecked(bool checked);
    void slotOutputChecked(bool checked);
    void slotHelpClicked();
    void slotPrintClicked();
};

#endif // ONETIMEPADGENERATOR_H
