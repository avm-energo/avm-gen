#pragma once

#include <QHttpMultiPart>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QUrl>
#include <avm-gen/error.h>
#include <avm-gen/netip.h>

class GENLIB_EXPORT HttpEngine : public QObject
{
    Q_OBJECT

public:
    struct StringPieceMultipart
    {
        QString name;
        QString data;
    };

    struct FilePieceMultipart
    {
        QString filename;
        QString name;
    };

    /// \brief Constructs HttpEngine, registers metatypes, wires up SSL error handling and the timeout timer.
    explicit HttpEngine(QObject *parent = nullptr);

    /// \brief Destructor — Qt ownership cleans up m_networkManager
    ~HttpEngine();

    /// \brief Получить результаты запроса
    /// \details Отправить GET-запрос на HTTP-сервер и получить результаты в JSON массиве
    /// \param server - имя сервера в виде "https://www.example.com:80"
    /// \param query - строка запроса (query из примера https://ip.com/query?id=12&param=34
    /// \param args - строки дополнительных параметров (id=12 и param=34 из предыдущего примера)
    /// \return массив данных (QByteArray)
    QByteArray GetQueryInBA(const QString &server, const QString &query = "", const QStringList &args = QStringList());

    /*! \brief Builds a URL from \a ip, \a port and \a isSSL and delegates to GetQueryInBA().
     *  \details Returns an empty QByteArray on network error or timeout.
     */
    QByteArray GetQueryInBAByIP(
        NetIP ip, int port = 80, const QString &query = "", const QStringList &args = QStringList(), bool isSSL = true);

    /// \brief Получить результаты запроса
    /// \details Отправить GET-запрос на HTTP-сервер и получить результаты в JSON массиве
    /// \param ip - ip-адрес сервера
    /// \param port - порт http-сервера
    /// \param query - строка запроса (query из примера https://ip.com/query?id=12&param=34)
    /// \param args - строки дополнительных параметров (id=12 и param=34 из предыдущего примера)
    /// \param isSSL - флаг, означающий запрос https (isSSL=true) или http (isSSL=false)
    /// \return JSON документ
    QJsonDocument GetQuery(
        NetIP ip, int port = 80, const QString &query = "", const QStringList &args = QStringList(), bool isSSL = true);

    /// \brief Получить файл
    /// \details Отправить GET-запрос на HTTP-сервер и получить результат в виде файла
    /// \param server - имя сервера
    /// \param query - строка запроса (query из примера https://ip.com/query?id=12&param=34)
    /// \param args - строки дополнительных параметров (id=12 и param=34 из предыдущего примера)
    /// \return имя временного файла, полученного от сервера
    QString GetFile(const QString &server, const QString &query = "", const QStringList &args = QStringList());

    /// \brief Получить файл
    /// \details Отправить GET-запрос на HTTP-сервер и получить результат в виде файла
    /// \param ip - ip-адрес сервера
    /// \param port - порт http-сервера
    /// \param query - строка запроса (query из примера https://ip.com/query?id=12&param=34)
    /// \param args - строки дополнительных параметров (id=12 и param=34 из предыдущего примера)
    /// \param isSSL - флаг, означающий запрос https (isSSL=true) или http (isSSL=false)
    /// \return имя временного файла, полученного от сервера
    QString GetFile(
        NetIP ip, int port = 80, const QString &query = "", const QStringList &args = QStringList(), bool isSSL = true);

    /*! \brief Sends a multipart-form POST to the given IP/port, returns the JSON response.
     *  \param ip     Target server IP address.
     *  \param port   Target port.
     *  \param query  Query path component.
     *  \param list   Items to include in the multipart body (StringPieceMultipart,
     *                FilePieceMultipart, or raw QByteArray wrapped in QVariant).
     *  \param isSSL  Use HTTPS when true, HTTP otherwise.
     *  \return JSON document
     */
    QJsonDocument PostQuery(
        NetIP ip, int port = 80, const QString &query = "", const QList<QVariant> &list = {}, bool isSSL = true);

    /// \brief Sets the request timeout.  Default is 10 000 ms; call before issuing a request.
    void setTimeoutTimer(int msec = 10000);

public slots:
    /// \brief Aborts the current download by setting the abort flag and emitting FinishReply().
    void CancelDownload();

signals:
    void BytesOverall(quint64 bytes);
    void BytesRead(quint64 bytes);
    void FinishReply();

private slots:
    /// \brief Forwards download-progress numbers as BytesOverall / BytesRead signals.
    void DownloadProgress(quint64 bytesreceived, quint64 bytesoverall);

    /*! \brief Slot called when the network reply finishes.
     *  \details Stops the timeout timer, handles errors, redirects, and marks the
     *           request as no longer busy so the synchronous wait-loop in GetQueryInBA()
     *           can exit.
     */
    void HttpFinished();

    /*! \brief Slot called when new data arrives from the network reply.
     *  \details Incrementally appends data to m_httpData, which reduces peak RAM
     *           usage compared to reading everything in HttpFinished().
     */
    void HttpReadyRead();

#ifndef QT_NO_SSL
    /// \brief Slot called when SSL errors occur; logs them and continues (permissive mode).
    void SSLErrors(QNetworkReply *, const QList<QSslError> &errors);
#endif
    /// \brief Slot called on network error; logs the specific error and calls CancelDownload().
    void HttpError(QNetworkReply::NetworkError code);

private:
    QUrl m_url;
    QNetworkAccessManager *m_networkManager;
    QByteArray m_httpData;
    bool m_httpRequestAborted;
    bool ReqBusy;
    int m_filePieceMultipartId, m_stringPieceMultipartId;
    QTimer *m_timeoutTimer;

    /*! \brief Sends a GET request to \a url and wires up all necessary reply signals.
     *  \details Also stores \a url in m_url so that relative redirects can be resolved
     *           correctly inside HttpFinished().
     */
    void Request(const QUrl &url);

    /*! \brief Sends a multipart POST request to \a url with \a body as the payload.
     *  \note Ownership of \a body is transferred to the reply; do not delete it manually.
     */
    void Post(const QUrl &url, QHttpMultiPart *body);
};

Q_DECLARE_METATYPE(HttpEngine::FilePieceMultipart)
Q_DECLARE_METATYPE(HttpEngine::StringPieceMultipart)
