/*
 *  FTP client based on https://github.com/Ceytrain/FTP-Client-With-QTcpSocket
 *  QT-ized by Evel
 *
 *  Simple usage:
 *  1. FtpEngine *engine = new FtpEngine;
 *  2. connectToHost(NetIP ip, port, username, password);
 *  3. setPath(remote, local);
 *  4. engine->moveToThread(thr);
 *  5. connect(thr, started, engine, get);
 *  6. connect(engine, finished, smth slot);
 *  7. thr.start();
 */

#pragma once

#include <QTcpSocket>
#include <gen/error.h>
#include <gen/gen_export.h>
#include <gen/netip.h>

class GENLIB_EXPORT FtpEngine : public QObject
{
    Q_OBJECT

public:
    explicit FtpEngine(QObject *parent = nullptr);
    ~FtpEngine();

    Error::Msg connectToHost(NetIP ip, quint16 port, const QString &user = "", const QString &password = "");
    Error::Msg list(QList<QString> &fileList);
    void setPath(const QString &remoteFile, const QString &localFile);
    void close();

public slots:
    void CancelDownload();
    void get();

signals:
    void BytesOverall(quint64 bytes);
    void BytesRead(quint64 bytes);
    void error(Error::Msg);
    void finished();

private slots:
    void DownloadProgress(quint64 bytesreceived, quint64 bytesoverall);

private:
    QTcpSocket *m_controlSocket, *m_dataSocket;
    QString m_path, m_downloadPath;

    void sendCommand(const QString &command);
    Error::Msg pasvMode();
};
