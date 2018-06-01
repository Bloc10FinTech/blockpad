#include "fileencryptionwgt.h"
#include "ui_onetimepadgeneratorwgt.h"
#include "global.h"
#include <QFileDialog>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QtConcurrent>
#include <QDesktopWidget>
#include <QStyle>
#define defSuccessEncrypt "Encryption completed successfully!"
#define defSuccessDecrypt "Decryption completed successfully!"

FileEncryptionWgt::FileEncryptionWgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OneTimePadGeneratorWgt)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    Utilities::setAppFamilyFont(ui->labelTitleOneTimePad, 16,QFont::Bold);
    //in begin encrypt
    {
        ui->radioButtonEncrypt->setChecked(true);
        ui->groupBoxDeCrypt->hide();
        adjustSize();
    }
#ifdef __linux__
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            QSize(this->size()),
            qApp->desktop()->availableGeometry()
        )
    );
#endif
    //signals/slots connection
    {
        connect(ui->radioButtonEncrypt, &QRadioButton::clicked,
                this, &FileEncryptionWgt::slotEncryptChoosed);
        connect(ui->radioButtonDecrypt, &QRadioButton::clicked,
                this, &FileEncryptionWgt::slotDecryptChoosed);

        QList<QPushButton *> buttons = {ui->pushButtonEncryptedFileDecrypt,
                                       ui->pushButtonEncryptedFileEncrypt,
                                       ui->pushButtonKeyFileDecrypt,
                                       ui->pushButtonKeyFileEncrypt,
                                       ui->pushButtonOriginalFileDecrypt,
                                       ui->pushButtonOriginalFileEncrypt};

        foreach( auto btn, buttons)
        {
            connect(btn, &QPushButton::clicked,
                    this,&FileEncryptionWgt::slotGetFileName);
        }

        connect(ui->pushButtonEncrypt, &QPushButton::clicked,
                this, &FileEncryptionWgt::slotEncryptClicked);

        connect(ui->pushButtonDecrypt, &QPushButton::clicked,
                this, &FileEncryptionWgt::slotDecryptClicked);

        connect(&futureWatcher, &QFutureWatcher<QString>::finished,
                this, &FileEncryptionWgt::slotEncryptDecryptFinished);
    }
}

void FileEncryptionWgt::slotEncryptDecryptFinished()
{
    auto de_en_result = futureWatcher.result();
    ui->labelStatus->clear();
    ui->groupBoxMain->setEnabled(true);
    if (de_en_result == defSuccessEncrypt
            ||
        de_en_result == defSuccessDecrypt)
        QMessageBox::information(this, "BlocPad",
                                 de_en_result);
    else
        QMessageBox::critical(this, "BlocPad",
                              de_en_result);
}

void FileEncryptionWgt::slotGetFileName()
{
    bool bSaveFile = false;
    if((ui->radioButtonEncrypt->isChecked()
            &&
       sender() == (QObject *)ui->pushButtonEncryptedFileEncrypt)
            ||
       (ui->radioButtonDecrypt->isChecked()
            &&
        sender() == (QObject *)ui->pushButtonOriginalFileDecrypt))
    {
        bSaveFile = true;
    }
    QString fileName;
    if(bSaveFile)
        fileName = QFileDialog::getSaveFileName(this, tr("Save File"));
    else
        fileName = QFileDialog::getOpenFileName(this, tr("Open File"));

    QLineEdit * edit;
    //fill edit
    {
        auto parentSender = ((QPushButton*)sender())->parentWidget();
        auto editList = parentSender->findChildren<QLineEdit *>();
        edit = editList.first();
    }
    edit->setText(fileName);
    if(ui->radioButtonEncrypt->isChecked()
            &&
       sender() == (QObject *)ui->pushButtonOriginalFileEncrypt)
    {
        ui->lineEditEncryptedFileEncrypt->setText(fileName + ".en");
    }

    if(ui->radioButtonDecrypt->isChecked()
            &&
        sender() == (QObject *)ui->pushButtonEncryptedFileDecrypt)
    {
        ui->lineEditOriginalFileDecrypt->setText(fileName + ".orig");
    }
}

void FileEncryptionWgt::slotEncryptClicked()
{
    ui->labelStatus->setText("Encryption in process. Do not close the windows until encryption is complete.");
    ui->groupBoxMain->setEnabled(false);
    futureWatcher.setFuture(QtConcurrent::run(this, &FileEncryptionWgt::encryptKeyFile));
}

void FileEncryptionWgt::slotDecryptClicked()
{
    ui->labelStatus->setText("Decryption in process. Do not close the windows until decryption is complete.");
    ui->groupBoxMain->setEnabled(false);
    futureWatcher.setFuture(QtConcurrent::run(this, &FileEncryptionWgt::decryptKeyFile));
}

QString FileEncryptionWgt::decryptKeyFile()
{
    QString nameOriginal = ui->lineEditOriginalFileDecrypt->text();
    QString nameKey = ui->lineEditKeyFileDecrypt->text();
    QString nameEncrypted = ui->lineEditEncryptedFileDecrypt->text();

    if(nameOriginal.isEmpty()
            ||
       nameKey.isEmpty()
            ||
       nameEncrypted.isEmpty())
    {
        return QString("Please fill in all the fields!");
    }

    QFile fileEncrypted(nameEncrypted);
    if(!fileEncrypted.open(QIODevice::ReadOnly))
    {
        return "Can not open file - " + nameEncrypted + "!";
    }

    QFile fileKey(nameKey);
    if(!fileKey.open(QIODevice::ReadOnly))
    {
        return QString("Can not open file - " + nameKey + "!");
    }

    if(fileEncrypted.size() == 0 )
    {
        return QString("Size of encrypted file must be different from 0");
    }

    if(fileEncrypted.size() > fileKey.size())
    {
        return QString("Size of key-file smaller then size of encrypted file - please choose other key file");
    }

    QByteArray baEncrypted = fileEncrypted.readAll();
    QByteArray baKey(baEncrypted.size(), ' ');
    //fill baKey
    {
        QByteArray dataBA = fileKey.readAll();
        int shift = 100;
        baKey.append(dataBA.data()+shift%fileEncrypted.size(),
                     fileEncrypted.size() - shift%fileEncrypted.size());
        baKey.append(dataBA.data(),
                     shift%fileEncrypted.size());
    }
    QByteArray baOriginal(baEncrypted.size(), ' ');
    //fill baEncrypted
    {
        for(int i=0; i<baEncrypted.size(); i++)
        {
            char symbol = baEncrypted[i]^baKey[i];
            baOriginal[i] = symbol;
        }
    }

    QFile fileOriginal(nameOriginal);
    if(!fileOriginal.open(QIODevice::WriteOnly))
    {
        return QString("Can not open file - " + nameOriginal + "!");
    }

    fileOriginal.write(baOriginal);

    fileEncrypted.close();
    fileOriginal.close();
    fileKey.close();
    return defSuccessDecrypt;
}

QString FileEncryptionWgt::encryptKeyFile()
{
    QString nameOriginal = ui->lineEditOriginalFileEncrypt->text();
    QString nameKey = ui->lineEditKeyFileEncrypt->text();
    QString nameEncrypted = ui->lineEditEncryptedFileEncrypt->text();

    if(nameOriginal.isEmpty()
            ||
       nameKey.isEmpty()
            ||
       nameEncrypted.isEmpty())
    {
        return QString("Please fill in all the fields!");
    }

    QFile fileOriginal(nameOriginal);
    if(!fileOriginal.open(QIODevice::ReadOnly))
    {
        return QString("Can not open file - " + nameOriginal + "!");
    }

    QFile fileKey(nameKey);
    if(!fileKey.open(QIODevice::ReadOnly))
    {
        return QString("Can not open file - " + nameKey + "!");
    }

    if(fileOriginal.size() == 0 )
    {
        return QString("Size of original file must be different from 0");
    }

    if(fileOriginal.size() > fileKey.size())
    {
        return QString("Size of key-file smaller then size of original file - please choose other key file");
    }

    QByteArray baOriginal = fileOriginal.readAll();
    QByteArray baKey(baOriginal.size(), ' ');
    //fill baKey
    {
        QByteArray dataBA = fileKey.readAll();
        int shift = 100;
        baKey.append(dataBA.data()+shift%fileOriginal.size(),
                     fileOriginal.size() - shift%fileOriginal.size());
        baKey.append(dataBA.data(),
                     shift%fileOriginal.size());
    }
    QByteArray baEncrypted(baOriginal.size(), ' ');
    //fill baEncrypted
    {
        for(int i=0; i<baOriginal.size(); i++)
        {
            char symbol = baOriginal[i]^baKey[i];
            baEncrypted[i] = symbol;
        }
    }

    QFile fileEncrypted(nameEncrypted);

    if(!fileEncrypted.open(QIODevice::WriteOnly))
    {
        return "Can not open file - " + nameEncrypted + "!";
    }

    fileEncrypted.write(baEncrypted);

    fileEncrypted.close();
    fileOriginal.close();
    fileKey.close();
    return defSuccessEncrypt;
}

void FileEncryptionWgt::slotEncryptChoosed()
{
    ui->groupBoxDeCrypt->hide();
    ui->groupBoxEncrypt->show();
}

void FileEncryptionWgt::slotDecryptChoosed()
{
    ui->groupBoxEncrypt->hide();
    ui->groupBoxDeCrypt->show();
}

FileEncryptionWgt::~FileEncryptionWgt()
{
    delete ui;
}
