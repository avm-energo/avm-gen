#include <QUrl>
#include <gen/files.h>
#include <gen/httpengine.h>
#include <gen/stdfunc.h>

HttpEngine::HttpEngine(QObject *parent) : QObject(parent)
{
    m_networkManager = new QNetworkAccessManager(this);
#ifndef QT_NO_SSL
    connect(m_networkManager, &QNetworkAccessManager::sslErrors, this, &HttpEngine::SSLErrors);
#endif
    m_httpRequestAborted = false;
}

HttpEngine::~HttpEngine()
{
}

void HttpEngine::Request(const QUrl &url)
{
    QNetworkReply *reply = m_networkManager->get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::downloadProgress, this, &HttpEngine::DownloadProgress);
    connect(reply, &QNetworkReply::finished, this, &HttpEngine::HttpFinished);
    connect(reply, &QIODevice::readyRead, this, &HttpEngine::HttpReadyRead);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(HttpError(QNetworkReply::NetworkError)));
    connect(this, &HttpEngine::FinishReply, reply, &QNetworkReply::abort);
}

void HttpEngine::CancelDownload()
{
    m_httpRequestAborted = true;
    emit FinishReply();
}

void HttpEngine::DownloadProgress(quint64 bytesreceived, quint64 bytesoverall)
{
    emit BytesOverall(bytesoverall);
    emit BytesRead(bytesreceived);
}

void HttpEngine::HttpFinished()
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

void HttpEngine::HttpReadyRead()
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
void HttpEngine::SSLErrors(QNetworkReply *, const QList<QSslError> &errors)
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

void HttpEngine::HttpError(QNetworkReply::NetworkError code)
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

QJsonDocument HttpEngine::GetQuery(NetIP ip, int port, const QString &query, const QStringList &args, bool isSSL)
{
    return QJsonDocument::fromJson(GetQueryInBA(ip, port, query, args, isSSL));
}

QString HttpEngine::GetFile(NetIP ip, int port, const QString &query, const QStringList &args, bool isSSL)
{
    return Files::SaveToTempFile(GetQueryInBA(ip, port, query, args, isSSL));
}

QByteArray HttpEngine::GetQueryInBA(NetIP ip, int port, const QString &query, const QStringList &args, bool isSSL)
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
