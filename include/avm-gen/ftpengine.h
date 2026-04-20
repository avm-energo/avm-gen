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
#include <avm-gen/error.h>
#include <avm-gen/gen_export.h>
#include <avm-gen/netip.h>

class GENLIB_EXPORT FtpEngine : public QObject
{
    Q_OBJECT

public:
    /// \brief Constructs FtpEngine and allocates control and data TCP sockets as children.
    explicit FtpEngine(QObject *parent = nullptr);

    ~FtpEngine();

    /*! \brief Connects to the FTP server and authenticates with \a user / \a password.
     *  \param ip        Server IP address.
     *  \param port      Control-channel port (usually 21).
     *  \param user      Login name; skips authentication when empty.
     *  \param password  Login password.
     *  \return Error::Msg::NoError on success, PswCheckError or GeneralError otherwise.
     */
    Error::Msg connectToHost(NetIP ip, quint16 port, const QString &user = "", const QString &password = "");

    /*! \brief Fetches a directory listing via the LIST command in passive mode.
     *  \param fileList Output list populated with one entry per directory line.
     *  \return Error::Msg::NoError on success, GeneralError otherwise.
     */
    Error::Msg list(QList<QString> &fileList);

    /*! \brief Sets the remote file path and the local destination path for the next get() call.
     *  \param remoteFile Path on the FTP server.
     *  \param localFile  Full path where the downloaded file will be saved.
     */
    void setPath(const QString &remoteFile, const QString &localFile);

    /// \brief Closes both the control and data sockets.
    void close();

public slots:
    /// \brief Slot that emits error(Cancelled) to abort an in-progress download.
    void CancelDownload();

    /*! \brief Downloads the remote file set by setPath() into the local path.
     *  \details Uses passive mode (RETR command).  Emits finished() on success or
     *           error(GeneralError) on failure.
     */
    void get();

signals:
    void BytesOverall(quint64 bytes);
    void BytesRead(quint64 bytes);
    void error(Error::Msg);
    void finished();

private slots:
    /// \brief Slot that forwards download progress as BytesOverall / BytesRead signals.
    void DownloadProgress(quint64 bytesreceived, quint64 bytesoverall);

private:
    QTcpSocket *m_controlSocket, *m_dataSocket;
    QString m_path, m_downloadPath;

    /*! \brief Sends a raw FTP command string over the control socket.
     *  \param command Full command text including the trailing CRLF (e.g., "PASV\r\n").
     *  \details BUG FIX: the original code passed ba.data() (null-terminated char*)
     *           to QTcpSocket::write(), which works for ASCII but would truncate at
     *           embedded null bytes.  Using write(ba) passes length explicitly.
     */
    void sendCommand(const QString &command);

    /*! \brief Sends PASV and opens the data socket to the address returned by the server.
     *  \details Parses the six-number address/port tuple from the 227 response.
     *  \return Error::Msg::NoError when the data connection is established,
     *          GeneralError otherwise.
     */
    Error::Msg pasvMode();
};
