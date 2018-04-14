#ifndef ONETIMEPADGENERATORWGT_H
#define ONETIMEPADGENERATORWGT_H

#include <QWidget>
#include <QFutureWatcher>

namespace Ui {
class OneTimePadGeneratorWgt;
}

class OneTimePadGeneratorWgt : public QWidget
{
    Q_OBJECT

public:
    explicit OneTimePadGeneratorWgt(QWidget *parent = 0);
    ~OneTimePadGeneratorWgt();

private:
    Ui::OneTimePadGeneratorWgt *ui;
    QFutureWatcher<QString> futureWatcher;
    QString encryptKeyFile();
    QString decryptKeyFile();
private slots:
    void slotEncryptChoosed();
    void slotDecryptChoosed();
    void slotGetFileName();
    void slotEncryptClicked();
    void slotDecryptClicked();
    void slotEncryptDecryptFinished();
};

#endif // ONETIMEPADGENERATORWGT_H
