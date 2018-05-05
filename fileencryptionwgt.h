#ifndef ONETIMEPADGENERATORWGT_H
#define ONETIMEPADGENERATORWGT_H

#include <QWidget>
#include <QFutureWatcher>

namespace Ui {
class OneTimePadGeneratorWgt;
}

class FileEncryptionWgt : public QWidget
{
    Q_OBJECT

public:
    explicit FileEncryptionWgt(QWidget *parent = 0);
    ~FileEncryptionWgt();

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
