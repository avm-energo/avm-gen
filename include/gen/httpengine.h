#pragma once

#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <gen/error.h>
#include <gen/netip.h>

class HttpEngine : public QObject
{
    Q_OBJECT

public:
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
        IP ip, int port = 80, const QString &query = "", const QStringList &args = QStringList(), bool isSSL = true);
    /// \brief Получить результаты запроса
    /// \details Отправить GET-запрос на HTTP-сервер и получить результаты в JSON массиве
    /// \param ip - ip-адрес сервера
    /// \param port - порт http-сервера
    /// \param query - строка запроса (query из примера https://ip.com/query?id=12&param=34
    /// \param args - строки дополнительных параметров (id=12 и param=34 из предыдущего примера)
    /// \param isSSL - флаг, означающий запрос https (isSSL=true) или http (isSSL=false)
    /// \return JSON документ
    QJsonDocument GetQuery(
        IP ip, int port = 80, const QString &query = "", const QStringList &args = QStringList(), bool isSSL = true);
    /// \brief Получить файл
    /// \details Отправить GET-запрос на HTTP-сервер и получить результат в виде файла
    /// \param ip - ip-адрес сервера
    /// \param port - порт http-сервера
    /// \param query - строка запроса (query из примера https://ip.com/query?id=12&param=34
    /// \param args - строки дополнительных параметров (id=12 и param=34 из предыдущего примера)
    /// \param isSSL - флаг, означающий запрос https (isSSL=true) или http (isSSL=false)
    /// \return имя временного файла, полученного от сервера
    QString GetFile(
        IP ip, int port = 80, const QString &query = "", const QStringList &args = QStringList(), bool isSSL = true);

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

    void Request(const QUrl &url);
};
