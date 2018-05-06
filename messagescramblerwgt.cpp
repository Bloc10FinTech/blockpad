#include "messagescramblerwgt.h"
#include "ui_messagescramblerwgt.h"
#include <QtConcurrent>
#include <QMessageBox>
#include <QRandomGenerator>
#include <random>
#include "global.h"

MessageScramblerWgt::MessageScramblerWgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageScramblerWgt)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    Utilities::setAppFamilyFont(ui->labelTitle, 16,QFont::Bold);
    slotEncryptDecryptChanged();
    //signals-slots connects
    {
        connect(ui->radioButtonDecrypt, &QRadioButton::clicked,
                this, &MessageScramblerWgt::slotEncryptDecryptChanged);
        connect(ui->radioButtonEncrypt, &QRadioButton::clicked,
                this, &MessageScramblerWgt::slotEncryptDecryptChanged);
        connect(ui->pushButtonGenerateOneTimePad, &QPushButton::clicked,
                this, &MessageScramblerWgt::sigGenerateOneTimePad);
        connect(ui->pushButtonEncryptDecrypt, &QPushButton::clicked,
                this, &MessageScramblerWgt::slotEncryptDecryptStarted);
        connect(&futureWatcher, &QFutureWatcher<QByteArray>::finished,
                this, &MessageScramblerWgt::slotEncryptDecryptFinished);
    }
}

QByteArray MessageScramblerWgt::EncryptDecrypt()
{
    QByteArray baAfter;
    QByteArray baBefore;
    if(ui->radioButtonEncrypt->isChecked())
        baBefore = ui->plainTextEditMessageBefore->
            toPlainText().toUtf8();
    else
        baBefore = QByteArray::fromHex(ui->plainTextEditMessageBefore->
            toPlainText().toLatin1());

    auto lengthKey = baBefore.size();
    QByteArray largeOneTimePad(lengthKey, ' ');

    //generate largeOneTimePad
    {
        QByteArray allSymbols = "0123456789abcdefghjklmnpqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ@#$%*&{}[]()/'\"\\`~,;:.!?<>^+-=_";
        std::string strSeed = ui->lineEditOneTimePad->text().toStdString();
        std::seed_seq seed(strSeed.begin(), strSeed.end());
        QRandomGenerator randomGenerator(seed);
        for (int iChar=0; iChar<lengthKey; iChar++)
        {
            int index = randomGenerator.bounded(allSymbols.size());
            largeOneTimePad[iChar]= allSymbols[index];
        }
    }

    // encrypt/decrypt
    {

        //fill baAfter
        {
            for(int i=0; i<baBefore.size(); i++)
            {
                char symbol = baBefore[i]^largeOneTimePad[i];
                baAfter[i] = symbol;
            }
        }
    }

    return baAfter;
}

void MessageScramblerWgt::slotEncryptDecryptStarted()
{
    if(ui->lineEditOneTimePad->text().isEmpty())
    {
        QMessageBox::critical(this, windowTitle(),
                              "Please, input one-time pad!");
        ui->lineEditOneTimePad->setFocus();
        return;
    }

    if(ui->plainTextEditMessageBefore->toPlainText().isEmpty())
    {
        if(ui->radioButtonEncrypt->isChecked())
            QMessageBox::critical(this, windowTitle(),
                                  "Please, input original message!");
        else
            QMessageBox::critical(this, windowTitle(),
                                  "Please, input encrypted message!");
        ui->plainTextEditMessageBefore->setFocus();
        return;
    }

    QString statusText;
    if (ui->radioButtonEncrypt->isChecked())
        statusText = "Encryption";
    else
        statusText = "Decryption";
    statusText += " in process. Do not close the windows until encryption is complete.";
    ui->labelStatus->setText(statusText);
    ui->groupBoxEncryptDecrypt->setEnabled(false);
    futureWatcher.setFuture(QtConcurrent::run(this, &MessageScramblerWgt::EncryptDecrypt));
}

void MessageScramblerWgt::slotEncryptDecryptFinished()
{
    auto baAfter = futureWatcher.result();
    ui->labelStatus->clear();
    ui->groupBoxEncryptDecrypt->setEnabled(true);
    if(ui->radioButtonEncrypt->isChecked())
        ui->plainTextEditMessageAfter->setPlainText(QString(baAfter.toHex()));
    else
        ui->plainTextEditMessageAfter->setPlainText(QString::fromUtf8(baAfter));
}

void MessageScramblerWgt::slotEncryptDecryptChanged()
{
    if(ui->radioButtonEncrypt->isChecked())
    {
        ui->labelAfter->setText("Encrypted");
        ui->labelBefore->setText("Original");
        ui->pushButtonEncryptDecrypt->setIcon(QIcon("://Icons/padGeneratorEncrypt.png"));
    }
    else
    {
        ui->labelAfter->setText("Original");
        ui->labelBefore->setText("Encrypted");
        ui->pushButtonEncryptDecrypt->setIcon(QIcon("://Icons/padGeneratorDecrypt.png"));
    }
}

MessageScramblerWgt::~MessageScramblerWgt()
{
    delete ui;
}
