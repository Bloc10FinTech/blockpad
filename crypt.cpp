#include "crypt.h"
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include "global.h"
#include "stega/steganography.h"
Crypto::Crypto()
{
    //fill baKey
    {
        SteganoReaderWriter st;
        st.decode_img("://Icons/Password.png");
        QString str = st.read_data();
        baKey.append(str);
    }
}

Crypto::~Crypto()
{

}

QByteArray Crypto::generateIv()
{
    QByteArray res;
    QString allSymbols {"@#$%*&23456789abcdefghjkmnpqrstuvwxyzABCDEFGHJKMNPQRSTUVWXYZil1ILo0O{}[]()/'\"\\`~,;:.!?<>^+-=_"};
    for(int i=0; i<16; i++)
    {
        int index = qrand() % allSymbols.length();
        QChar nextChar = allSymbols.at(index);
        res.append(nextChar);
    }
    return res;
}

QList<QPair<QString, QString>> Crypto::listEmailPassws(QList<QString> &nameFiles)
{
    QList<QPair<QString, QString>> res;
    QList<QString> newNameFiles;
    foreach(auto name, nameFiles)
    {
       bool bSuccess = true;
       auto pair = pairEmailPassw(name, bSuccess);
       if(bSuccess)
       {
           res.append(pair);
           newNameFiles.append(name);
       }
    }
    nameFiles = newNameFiles;
    return res;
}

QPair<QString, QString> Crypto::pairEmailPassw(QString fileName, bool & bSuccess)
{
    unsigned char * defKey = reinterpret_cast<unsigned char *>(baKey.data());
    bSuccess = true;
    QPair<QString, QString> pair;
    QFile file(fileName);
    if(QFileInfo(fileName).suffix() != "bloc")
    {
        qDebug() << "wrong suffix of " + fileName;
        bSuccess = false;
        return pair;
    }
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "can not open file " + fileName + " to write";
        bSuccess = false;
        return pair;
    }
    QByteArray baAllCipher = file.readAll();
    if(baAllCipher.size() < 8)
    {
        qDebug() << "wrong size file - " + fileName;
        bSuccess = false;
        return pair;
    }
    QByteArray baIv = baAllCipher.left(16);
    unsigned char * defIV = reinterpret_cast<unsigned char *>(baIv.data());
    baAllCipher.remove(0, 16);
    if(!(ctxDecrypt = EVP_CIPHER_CTX_new()))
    {
        qDebug() << "decrypt error";
        return pair;
    }
    if(1 != EVP_DecryptInit_ex(ctxDecrypt, EVP_aes_256_cfb128(), NULL, defKey, defIV))
    {
        qDebug() << "decrypt error";
        return pair;
    }
    int pos = 0;

    QStringList texts;
    texts << ""<< "" << "";
    QList<int> sizes;
    sizes << 0<< 0 <<0;
    for(int i=0; i<3; i++)
    {
        //fill sizes[i]
        {
            int plaintext_len;
            unsigned char *ciphertext
                    = reinterpret_cast<unsigned char *>(baAllCipher.data() + pos);
            int ciphertext_len = sizeof(int);
            unsigned char *plaintext = new unsigned char[ciphertext_len + 8];
            if(1 != EVP_DecryptUpdate(ctxDecrypt, plaintext, &plaintext_len, ciphertext, ciphertext_len))
            {
                qDebug() << "decrypt error";
                return pair;
            }
            sizes[i] = *((int *)plaintext);
            pos += sizeof(int);
            delete[] plaintext;
        }
        //fill texts[i]
        {
            int plaintext_len;
            unsigned char *ciphertext
                    = reinterpret_cast<unsigned char *>(baAllCipher.data() + pos);
            int ciphertext_len = sizes[i];
            unsigned char *plaintext = new unsigned char[ciphertext_len + 8];
            if(1 != EVP_DecryptUpdate(ctxDecrypt, plaintext, &plaintext_len, ciphertext, ciphertext_len))
            {
                qDebug() << "decrypt error";
                return pair;
            }
            QByteArray ba;
            ba.append((const char *)plaintext, plaintext_len);
            texts[i] = ba;
            pos += sizes[i];
            delete[] plaintext;
        }
    }
    pair.first = texts[1];
    pair.second = texts[2];
    EVP_CIPHER_CTX_free(ctxDecrypt);
    return pair;
}

void Crypto::encrypt(   QByteArray ba_plain_text,
                        QByteArray & ba_cipher_text,
                        QByteArray & ba_Iv)
{
    unsigned char * defKey = reinterpret_cast<unsigned char *>(baKey.data());
    ba_cipher_text.clear();
    unsigned char *plaintext = reinterpret_cast<unsigned char *>(ba_plain_text.data());
    int plaintext_len = ba_plain_text.size();
    unsigned char *ciphertext = new unsigned char[ba_plain_text.size() + 8];

    int len;

    int ciphertext_len;
    ba_Iv = generateIv();
    unsigned char * defIV = reinterpret_cast<unsigned char *>(ba_Iv.data());
    /* Create and initialise the context */
    if(!(ctxEncrypt = EVP_CIPHER_CTX_new()))
    {
        qDebug() << "encrypt error";
        return;
    }

    /* Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits */
    if(1 != EVP_EncryptInit_ex(ctxEncrypt, EVP_aes_256_cfb128(), NULL, defKey, defIV))
    {
        qDebug() << "encrypt error";
        return;
    }

    /* Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(ctxEncrypt, ciphertext, &len, plaintext, plaintext_len))
    {
        qDebug() << "encrypt error";
        return;
    }
    ciphertext_len = len;

//    /* Finalise the encryption. Further ciphertext bytes may be written at
//     * this stage.
//     */
//    if(1 != EVP_EncryptFinal_ex(ctxEncrypt, ciphertext + len, &len))
//        qDebug() << "encrypt error";
//    ciphertext_len += len;

    ba_cipher_text.append((const char *)ciphertext, ciphertext_len);
    delete[] ciphertext;
    EVP_CIPHER_CTX_free(ctxEncrypt);
}


void Crypto::decrypt(   QByteArray ba_cipher_text,
                        QByteArray & ba_plain_text)
{
    unsigned char * defKey = reinterpret_cast<unsigned char *>(baKey.data());
    if(ba_cipher_text.size() < 8)
    {
        qDebug() << "wrong size ba_cipher_text";
        return;
    }
    QByteArray baIv = ba_cipher_text.left(16);
    ba_cipher_text.remove(0, 16);
    unsigned char * defIV = reinterpret_cast<unsigned char *>(baIv.data());
    ba_plain_text.clear();
    unsigned char *ciphertext = reinterpret_cast<unsigned char *>(ba_cipher_text.data());
    int ciphertext_len = ba_cipher_text.size();
    unsigned char *plaintext = new unsigned char[ba_cipher_text.size() + 8];

    int len;

    int plaintext_len;
    /* Create and initialise the context */
    if(!(ctxDecrypt = EVP_CIPHER_CTX_new()))
    {
        qDebug() << "decrypt error";
        return;
    }

    /* Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits */
    if(1 != EVP_DecryptInit_ex(ctxDecrypt, EVP_aes_256_cfb128(), NULL, defKey, defIV))
    {
        qDebug() << "decrypt error";
        return;
    }

    /* Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_DecryptUpdate(ctxDecrypt, plaintext, &len, ciphertext, ciphertext_len))
    {
        qDebug() << "decrypt error";
        return;
    }
    plaintext_len = len;

    /* Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
//    if(1 != EVP_DecryptFinal_ex(ctxDecrypt, plaintext + len, &len))
//        qDebug() << "decrypt error";
//    plaintext_len += len;

    ba_plain_text.append((const char *)plaintext, plaintext_len);
    delete[] plaintext;
    EVP_CIPHER_CTX_free(ctxDecrypt);
}
