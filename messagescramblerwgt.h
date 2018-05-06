#ifndef MESSAGESCRAMBLERWGT_H
#define MESSAGESCRAMBLERWGT_H

#include <QWidget>
#include <QFutureWatcher>
namespace Ui {
class MessageScramblerWgt;
}

class MessageScramblerWgt : public QWidget
{
    Q_OBJECT

public:
    explicit MessageScramblerWgt(QWidget *parent = 0);
    ~MessageScramblerWgt();

private:
    Ui::MessageScramblerWgt *ui;
    QFutureWatcher<QByteArray> futureWatcher;
    QByteArray EncryptDecrypt();
private slots:
    void slotEncryptDecryptChanged();
    void slotEncryptDecryptStarted();
    void slotEncryptDecryptFinished();
signals:
    void sigGenerateOneTimePad();
};

#endif // MESSAGESCRAMBLERWGT_H
