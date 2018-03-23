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
    QList<QPair<QString, QString> > listEmailPassws(QList<QString>& nameFiles);
    QPair<QString, QString> pairEmailPassw(QString fileName, bool &bSuccess);
    void encrypt(QByteArray ba_plain_text,
                 QByteArray &ba_cipher_text);
    void decrypt(QByteArray ba_cipher_text,
                 QByteArray & ba_plain_text);
    ~Crypto();
private:
    EVP_CIPHER_CTX *ctxEncrypt;
    EVP_CIPHER_CTX *ctxDecrypt;
    Crypto();
    //random 16 byte sequence
    QByteArray baKey;
    unsigned char * defKey;
    QByteArray baIV;
    unsigned char * defIV;
};
#endif // CRYPT_H
