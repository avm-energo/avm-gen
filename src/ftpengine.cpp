#include <QUrl>
#include <gen/files.h>
#include <gen/ftpengine.h>
#include <gen/stdfunc.h>

FtpEngine::FtpEngine(QObject *parent) : QObject(parent)
{
    m_controlSocket = new QTcpSocket(this);
    m_dataSocket = new QTcpSocket(this);
}

FtpEngine::~FtpEngine()
{
    if (m_controlSocket != nullptr)
        delete m_controlSocket;
    if (m_dataSocket != nullptr)
        delete m_dataSocket;
}

Error::Msg FtpEngine::connectToHost(NetIP ip, quint16 port, const QString &user, const QString &password)
{
    m_controlSocket->connectToHost(IP::toString(ip), port);
    if (m_controlSocket->waitForConnected())
    {
        m_controlSocket->waitForReadyRead();
        QByteArray ba = m_controlSocket->readAll();
        if (!user.isEmpty())
        {
            sendCommand("USER " + user + "\r\n");
            if (m_controlSocket->waitForReadyRead())
            {
                m_controlSocket->waitForReadyRead();
                ba = m_controlSocket->readAll();
                sendCommand("PASS " + password + "\r\n");
                if (m_controlSocket->waitForReadyRead())
                {
                    m_controlSocket->waitForReadyRead();
                    ba = m_controlSocket->readAll();
                    return Error::Msg::NoError;
                }
            }
            return Error::Msg::PswCheckError;
        }
        return Error::Msg::NoError;
    }
    else
        return Error::Msg::GeneralError;
}

Error::Msg FtpEngine::list(QList<QString> &fileList)
{
    QByteArray ba;
    if (pasvMode() == Error::Msg::NoError)
    {
        sendCommand("LIST\r\n");
        if (m_controlSocket->waitForReadyRead())
        {
            ba = m_controlSocket->readAll();
            if (m_dataSocket->waitForReadyRead())
            {
                ba = m_dataSocket->readAll();
            }
        }
    }
    return Error::Msg::GeneralError;
}

void FtpEngine::close()
{
    m_controlSocket->close();
    m_dataSocket->close();
}

void FtpEngine::Request(const QUrl &url)
{
    QNetworkReply *reply = m_networkManager->get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::downloadProgress, this, &FtpEngine::DownloadProgress);
    connect(reply, &QNetworkReply::finished, this, &FtpEngine::HttpFinished);
    connect(reply, &QIODevice::readyRead, this, &FtpEngine::HttpReadyRead);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(HttpError(QNetworkReply::NetworkError)));
    connect(this, &FtpEngine::FinishReply, reply, &QNetworkReply::abort);
}

void FtpEngine::CancelDownload()
{
    m_httpRequestAborted = true;
    emit FinishReply();
}

void FtpEngine::DownloadProgress(quint64 bytesreceived, quint64 bytesoverall)
{
    emit BytesOverall(bytesoverall);
    emit BytesRead(bytesreceived);
}

void FtpEngine::sendCommand(const QString &command)
{
    QByteArray ba = command.toUtf8();
    m_controlSocket->write(ba.data());
    m_controlSocket->waitForBytesWritten();
}

Error::Msg FtpEngine::pasvMode()
{
    QByteArray ba;
    sendCommand("PASV\r\n");
    if (m_controlSocket->waitForReadyRead())
    {
        m_controlSocket->waitForReadyRead();
        ba = m_controlSocket->readAll();
    }
    else
        return Error::Msg::GeneralError;

    // get IP & Port from bytearray
    QRegularExpression re;
    re.setPattern("[0-9]+");
    QRegularExpressionMatchIterator i = re.globalMatch(ba);
    QStringList list;
    while (i.hasNext())
    {
        list.append(i.next().captured(1));
    }
    if (list.size() > 5)
    {
        m_dataSocket->close();
        m_dataSocket->connectToHost(list.at(0) + "." + list.at(1) + "." + list.at(2) + "." + list.at(3),
            list.at(4).toInt() * 256 + list.at(5).toInt());
        if (m_dataSocket->waitForConnected())
            return Error::Msg::NoError;
    }
    return Error::Msg::GeneralError;
}

void FtpEngine::HttpFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (reply == nullptr)
    {
        qWarning() << "Bad http reply";
        ReqBusy = false;
        return;
    }
    if (m_httpRequestAborted)
    {
        reply->deleteLater();
        reply = Q_NULLPTR;
        ReqBusy = false;
        return;
    }

    if (reply->error())
    {
        qWarning() << "Download failed: " + reply->errorString();
        reply->deleteLater();
        m_httpRequestAborted = true;
        ReqBusy = false;
        return;
    }

    const QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    reply->deleteLater();
    reply = Q_NULLPTR;

    if (!redirectionTarget.isNull())
    {
        const QUrl redirectedUrl = m_url.resolved(redirectionTarget.toUrl());
        m_httpData.clear();
        Request(redirectedUrl);
        return;
    }
    ReqBusy = false;
}

void FtpEngine::HttpReadyRead()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (reply == nullptr)
    {
        m_httpRequestAborted = true;
        ReqBusy = false;
        return;
    }
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    m_httpData.append(reply->readAll());
}

#ifndef QT_NO_SSL
void FtpEngine::SSLErrors(QNetworkReply *, const QList<QSslError> &errors)
{
    QString errorString;
    foreach (const QSslError &error, errors)
    {
        if (!errorString.isEmpty())
            errorString += '\n';
        errorString += error.errorString();
    }
    qWarning() << "One or more SSL errors has occurred: " + errorString;
}
#endif

void FtpEngine::HttpError(QNetworkReply::NetworkError code)
{
    switch (code)
    {
    case QNetworkReply::ConnectionRefusedError:
        qWarning() << "Http connection refused";
        break;
    case QNetworkReply::RemoteHostClosedError:
        qWarning() << "Http remote host closed connection";
        break;
    case QNetworkReply::HostNotFoundError:
        qWarning() << "Http host not found";
        break;
    case QNetworkReply::TimeoutError:
        qWarning() << "Http connection timeout";
        break;
    case QNetworkReply::OperationCanceledError:
        qWarning() << "Http operation cancelled";
        break;
    default:
        qWarning() << "Http error, code " + QString::number(code);
        break;
    }
    CancelDownload();
}

QJsonDocument FtpEngine::GetQuery(NetIP ip, int port, const QString &query, const QStringList &args, bool isSSL)
{
    return QJsonDocument::fromJson(GetQueryInBA(ip, port, query, args, isSSL));
}

QString FtpEngine::GetFile(NetIP ip, int port, const QString &query, const QStringList &args, bool isSSL)
{
    return Files::SaveToTempFile(GetQueryInBA(ip, port, query, args, isSSL));
}

QByteArray FtpEngine::GetQueryInBA(NetIP ip, int port, const QString &query, const QStringList &args, bool isSSL)
{
    QString url = (isSSL) ? "https://" : "http://";
    url += IP::toString(ip) + ":" + QString::number(port) + "/" + query + "?";
    if (!args.isEmpty())
    {
        for (QString arg : args)
            url += arg + "&";
    }
    url.chop(1);
    const QString urlSpec = url.trimmed(); // removes whitespaces
    const QUrl newUrl = QUrl::fromUserInput(urlSpec);
    qDebug() << "Get query: " << urlSpec;
    if (!newUrl.isValid())
    {
        qWarning() << "Invalid url";
        return QByteArray();
    }
    m_httpData.clear();
    m_httpRequestAborted = false;
    ReqBusy = true;
    Request(newUrl);
    while ((ReqBusy) && (!m_httpRequestAborted))
        StdFunc::Wait();
    if (m_httpRequestAborted)
        return QByteArray();
    return m_httpData;
}
