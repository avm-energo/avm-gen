#pragma once

#include <QHttpMultiPart>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QUrl>
#include <gen/error.h>
#include <gen/netip.h>

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

    explicit HttpEngine(QObject *parent = nullptr);
    ~HttpEngine();

    /// \brief Получить результаты запроса
    /// \details Отправить GET-запрос на HTTP-сервер и получить результаты в JSON массиве
    /// \param ip - ip-адрес сервера
    /// \param port - порт http-сервера
    /// \param query - строка запроса (query из примера https://ip.com/query?id=12&param=34
    /// \param args - строки дополнительных параметров (id=12 и param=34 из предыдущего примера)
    /// \param isSSL - флаг, означающий запрос https (isSSL=true) или http (isSSL=false)
    /// \return массив данных (QByteArray)
    QByteArray GetQueryInBA(
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
    /// \param ip - ip-адрес сервера
    /// \param port - порт http-сервера
    /// \param query - строка запроса (query из примера https://ip.com/query?id=12&param=34)
    /// \param args - строки дополнительных параметров (id=12 и param=34 из предыдущего примера)
    /// \param isSSL - флаг, означающий запрос https (isSSL=true) или http (isSSL=false)
    /// \return имя временного файла, полученного от сервера
    QString GetFile(
        NetIP ip, int port = 80, const QString &query = "", const QStringList &args = QStringList(), bool isSSL = true);
    /// \brief Отправить запрос на сервер
    /// \details Отправить POST-запрос с данными на сервер и получить результаты в JSON массиве
    /// \param ip - ip-адрес сервера
    /// \param port - порт сервера
    /// \param query - строка запроса (query из примера https://ip.com/query)
    /// \param list - список данных для передачи (QList<QVariant>)
    /// \return JSON документ
    QJsonDocument PostQuery(
        NetIP ip, int port = 80, const QString &query = "", const QList<QVariant> &list = {}, bool isSSL = true);
    void setTimeoutTimer(int msec = 10000);

public slots:
    void CancelDownload();

signals:
    void BytesOverall(quint64 bytes);
    void BytesRead(quint64 bytes);
    void FinishReply();

private slots:
    void DownloadProgress(quint64 bytesreceived, quint64 bytesoverall);
    void HttpFinished();
    void HttpReadyRead();
#ifndef QT_NO_SSL
    void SSLErrors(QNetworkReply *, const QList<QSslError> &errors);
#endif
    void HttpError(QNetworkReply::NetworkError code);

private:
    QUrl m_url;
    QNetworkAccessManager *m_networkManager;
    QByteArray m_httpData;
    bool m_httpRequestAborted;
    bool ReqBusy;
    int m_filePieceMultipartId, m_stringPieceMultipartId;
    QTimer *m_timeoutTimer;

    void Request(const QUrl &url);
    void Post(const QUrl &url, QHttpMultiPart *body);
};

Q_DECLARE_METATYPE(HttpEngine::FilePieceMultipart)
Q_DECLARE_METATYPE(HttpEngine::StringPieceMultipart)
