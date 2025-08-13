#pragma once

#include <QObject>
#include <gen/gen_export.h>

class GENLIB_EXPORT Encryption : public QObject
{
    Q_OBJECT
public:
    Encryption(Encryption const &) = delete;
    void operator=(Encryption const &) = delete;

    static Encryption &getInstance();

    QString encrypt(const QString &source);
    QString decrypt(const QString &source);
    QString encryptRSA(const QString &source, const QByteArray &pubKey);
    QString decryptRSA(const QString &source, const QByteArray &privKey);

signals:

private:
    Encryption();

    QByteArray m_iv;
    QByteArray m_key;
};
