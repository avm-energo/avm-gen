#pragma once

#include <QJsonDocument>
#include <QTcpSocket>
#include <QUrl>
#include <gen/error.h>
#include <gen/netip.h>

class GENLIB_EXPORT FtpEngine : public QObject
{
    Q_OBJECT

public:
    explicit FtpEngine(QObject *parent = nullptr);
    ~FtpEngine();

    Error::Msg connectToHost(NetIP ip, quint16 port, const QString &user = "", const QString &password = "");
    Error::Msg list(QList<QString> &fileList);
    Error::Msg get(const QString &path, const QString &downloadPath);
    void close();

public slots:
    void CancelDownload();

signals:
    void BytesOverall(quint64 bytes);
    void BytesRead(quint64 bytes);
    void FinishReply();

private slots:
    void DownloadProgress(quint64 bytesreceived, quint64 bytesoverall);
    void ftpFinished();
    void ftpReadyRead();

private:
    QUrl m_url;
    QByteArray m_httpData;
    QTcpSocket *m_controlSocket, *m_dataSocket;
    bool m_httpRequestAborted;
    bool ReqBusy;

    void sendCommand(const QString &command);
    Error::Msg pasvMode();

    void Request(const QUrl &url);
};
