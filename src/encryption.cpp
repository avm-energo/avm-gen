#include <QCryptographicHash>
#include <QSysInfo>
#include <gen/encryption.h>
#include <qaesencryption.h>
#include <qrsaencryption.h>

Encryption &Encryption::getInstance()
{
    static Encryption instance;
    return instance;
}

QString Encryption::encrypt(const QString &source)
{
    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
    return encryption.encode(source.toLocal8Bit(), m_key, m_iv).toHex();
}

QString Encryption::decrypt(const QString &source)
{
    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
    QByteArray ba = QByteArray::fromHex(source.toLocal8Bit());
    QByteArray dest = encryption.decode(ba, m_key, m_iv);
    return QString(encryption.removePadding(dest));
}

QString Encryption::encryptRSA(const QString &source, const QByteArray &pubKey)
{
    QRSAEncryption encryption;
    return encryption.encode(source.toLocal8Bit(), pubKey).toHex();
}

QString Encryption::decryptRSA(const QString &source, const QByteArray &privKey)
{
    QRSAEncryption encryption;
    QByteArray ba = QByteArray::fromHex(source.toLocal8Bit());
    return QString(encryption.decode(ba, privKey));
}

Encryption::Encryption()
{
    m_key = QCryptographicHash::hash(QSysInfo::machineUniqueId().first(16), QCryptographicHash::Sha256);
    m_iv = QCryptographicHash::hash(QSysInfo::machineUniqueId().last(16), QCryptographicHash::Md5);
}
