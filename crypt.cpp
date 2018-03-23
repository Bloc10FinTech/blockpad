#include "crypt.h"
#include <QDebug>
#include <QMessageBox>
#include "global.h"

Crypto::Crypto()
{
    //fill defKey
    {
        QFile file("://Passwords/CryptoKey.txt");
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(nullptr,
                                  "BlockPad",
                                  "Can not open file with crypto key");
        }
        baKey = file.readLine();
        file.close();
        defKey = reinterpret_cast<unsigned char *>(baKey.data());
    }

    //fill defIv
    {
        QFile file("://Passwords/CryptoIv.txt");
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(nullptr,
                                  "BlockPad",
                                  "Can not open file with crypto Iv");
        }
        baIV = file.readLine();
        file.close();
        defIV = reinterpret_cast<unsigned char *>(baIV.data());
    }
}

Crypto::~Crypto()
{

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
    bSuccess = true;
    QPair<QString, QString> pair;
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "can not open file " + fileName + " to write";
        bSuccess = false;
        return pair;
    }
    QByteArray baAllCipher = file.readAll();
    if(!(ctxDecrypt = EVP_CIPHER_CTX_new()))
        qDebug() << "decrypt error";

    if(1 != EVP_DecryptInit_ex(ctxDecrypt, EVP_bf_cfb64(), NULL, defKey, defIV))
      qDebug() << "decrypt error";
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
                qDebug() << "decrypt error";
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
                qDebug() << "decrypt error";
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
                        QByteArray & ba_cipher_text)
{
    ba_cipher_text.clear();
    unsigned char *plaintext = reinterpret_cast<unsigned char *>(ba_plain_text.data());
    int plaintext_len = ba_plain_text.size();
    unsigned char *ciphertext = new unsigned char[ba_plain_text.size() + 8];

    int len;

    int ciphertext_len;
    /* Create and initialise the context */
    if(!(ctxEncrypt = EVP_CIPHER_CTX_new()))
        qDebug() << "encrypt error";

    /* Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits */
    if(1 != EVP_EncryptInit_ex(ctxEncrypt, EVP_bf_cfb64(), NULL, defKey, defIV))
        qDebug() << "encrypt error";

    /* Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(ctxEncrypt, ciphertext, &len, plaintext, plaintext_len))
      qDebug() << "encrypt error";
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
    ba_plain_text.clear();
    unsigned char *ciphertext = reinterpret_cast<unsigned char *>(ba_cipher_text.data());
    int ciphertext_len = ba_cipher_text.size();
    unsigned char *plaintext = new unsigned char[ba_cipher_text.size() + 8];

    int len;

    int plaintext_len;
    /* Create and initialise the context */
    if(!(ctxDecrypt = EVP_CIPHER_CTX_new()))
        qDebug() << "decrypt error";

    /* Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits */
    if(1 != EVP_DecryptInit_ex(ctxDecrypt, EVP_bf_cfb64(), NULL, defKey, defIV))
      qDebug() << "decrypt error";

    /* Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_DecryptUpdate(ctxDecrypt, plaintext, &len, ciphertext, ciphertext_len))
     qDebug() << "decrypt error";
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
