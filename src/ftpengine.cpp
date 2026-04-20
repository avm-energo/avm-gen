#include <QRegularExpressionMatchIterator>
#include <avm-gen/files.h>
#include <avm-gen/ftpengine.h>
#include <avm-gen/stdfunc.h>

FtpEngine::FtpEngine(QObject *parent) : QObject(parent)
{
    m_controlSocket = new QTcpSocket(this);
    m_dataSocket = new QTcpSocket(this);
}

FtpEngine::~FtpEngine() = default;

Error::Msg FtpEngine::connectToHost(NetIP ip, quint16 port, const QString &user, const QString &password)
{
    m_controlSocket->connectToHost(IP::toString(ip), port);
    if (m_controlSocket->waitForConnected())
    {
        m_controlSocket->waitForReadyRead();
        QByteArray ba = m_controlSocket->readAll();
        qDebug() << "CONNECT: " << ba;
        if (!user.isEmpty())
        {
            sendCommand("USER " + user + "\r\n");
            if (m_controlSocket->waitForReadyRead())
            {
                ba = m_controlSocket->readAll();
                qDebug() << "USER: " << ba;
                sendCommand("PASS " + password + "\r\n");
                if (m_controlSocket->waitForReadyRead())
                {
                    ba = m_controlSocket->readAll();
                    qDebug() << "PASS: " << ba;
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
            qDebug() << "LIST: " << ba;
            if (m_dataSocket->waitForReadyRead())
            {
                ba = m_dataSocket->readAll();
                static const QRegularExpression re("((.+?)(?=\\r\\n))");
                QRegularExpressionMatchIterator i = re.globalMatch(ba);
                while (i.hasNext())
                    fileList.append(i.next().captured(1));
                if (m_controlSocket->waitForReadyRead())
                {
                    ba = m_controlSocket->readAll();
                    qDebug() << "LIST: " << ba;
                }
                return Error::Msg::NoError;
            }
        }
    }
    return Error::Msg::GeneralError;
}

void FtpEngine::get()
{
    QByteArray ba;
    bool timeout = false;

    if (pasvMode() == Error::Msg::NoError)
    {
        sendCommand("RETR " + m_path + "\r\n");
        if (m_controlSocket->waitForReadyRead())
        {
            ba = m_controlSocket->readAll();
            qDebug() << "GET: " << ba;
            QFile file(m_downloadPath);
            if (file.open(QFile::WriteOnly))
            {
                while ((!ba.startsWith("226")) && !timeout)
                {
                    if (m_dataSocket->waitForReadyRead(1000))
                    {
                        ba = m_dataSocket->readAll();
                        qDebug() << ba.size() << " bytes read";
                        file.write(ba);
                        ba = m_controlSocket->readAll();
                        qDebug() << "GET: " << ba;
                    }
                    else
                    {
                        timeout = true;
                    }
                }
                file.close();
                emit finished();
                return;
            }
        }
    }
    emit error(Error::Msg::GeneralError);
}

void FtpEngine::setPath(const QString &remoteFile, const QString &localFile)
{
    m_path = remoteFile;
    m_downloadPath = localFile;
}

void FtpEngine::close()
{
    m_controlSocket->close();
    m_dataSocket->close();
}

void FtpEngine::CancelDownload()
{
    emit error(Error::Msg::Cancelled);
}

void FtpEngine::DownloadProgress(quint64 bytesreceived, quint64 bytesoverall)
{
    emit BytesOverall(bytesoverall);
    emit BytesRead(bytesreceived);
}

void FtpEngine::sendCommand(const QString &command)
{
    QByteArray ba = command.toUtf8();
    m_controlSocket->write(ba); // use QByteArray overload, not char*
    m_controlSocket->waitForBytesWritten();
}

Error::Msg FtpEngine::pasvMode()
{
    QByteArray ba;
    sendCommand("PASV\r\n");
    if (m_controlSocket->waitForReadyRead())
    {
        ba = m_controlSocket->readAll();
        qDebug() << "PASV: " << ba;
    }
    else
        return Error::Msg::GeneralError;

    // Parse the six numbers from the 227 response: "227 Entering Passive Mode (h1,h2,h3,h4,p1,p2)"
    static const QRegularExpression re("[0-9]+");
    QRegularExpressionMatchIterator i = re.globalMatch(ba);
    QStringList list;
    if (i.hasNext())
        i.next(); // skip the leading 227 status code
    while (i.hasNext())
        list.append(i.next().captured(0));

    if (list.size() > 5)
    {
        m_dataSocket->close();
        m_dataSocket->connectToHost(list.at(0) + "." + list.at(1) + "." + list.at(2) + "." + list.at(3),
            list.at(4).toInt() * 256 + list.at(5).toInt());
        if (m_dataSocket->waitForConnected())
            return Error::Msg::NoError;
    }
    else
    {
        qWarning() << "PASV response parsing failed: not enough address components";
        return Error::Msg::GeneralError;
    }
    return Error::Msg::GeneralError;
}
