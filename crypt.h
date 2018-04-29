#ifndef CRYPT_H
#define CRYPT_H
#include <openssl/blowfish.h>
#include <QByteArray>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <QList>
#include <QPair>
#include <QFile>
#include <QDir>
class Crypto
{
public:
    static Crypto & Instance()
    {
        static Crypto singleton;
        return singleton;
    }
    QList<QPair<QString, QString> > listEmailPassws(QList<QString>& nameFiles,
                                                    QMap<QString, QString> &fileIds,
                                                    QStringList &displayEmails);
    QPair<QString, QString> pairEmailPassw(QString fileName,
                                           bool &bSuccess,
                                           QString & id);
    void encrypt(QByteArray ba_plain_text,
                 QByteArray &ba_cipher_text,
                 QByteArray &ba_Iv);
    void decrypt(QByteArray ba_cipher_text,
                 QByteArray & ba_plain_text);
    ~Crypto();
private:
    EVP_CIPHER_CTX *ctxEncrypt;
    EVP_CIPHER_CTX *ctxDecrypt;
    Crypto();
    //random 16 byte sequence
    QByteArray baKey;
    QByteArray baIV;
    QByteArray generateIv();
};
#endif // CRYPT_H
