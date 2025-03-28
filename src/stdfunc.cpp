#include "gen/stdfunc.h"

#include <QCoreApplication>
#include <QDataStream>
#include <QDateTime>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QHostAddress>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <QTcpSocket>
#include <QTextStream>
#include <QThread>
#include <QTimer>
#include <gen/error.h>
#include <gen/pch.h>

QString StdFunc::s_homeDir = "";       // Рабочий каталог программы
QString StdFunc::s_systemHomeDir = ""; // Системный каталог программы
decltype(StdFunc::s_state) StdFunc::s_state {};

QString StdFunc::s_deviceIP = "";
QString StdFunc::s_OrganizationString = "";
int StdFunc::m_tuneRequestCount = 0;

/*! \brief Initialization function for static class fields.
 *  \details Initialize next fields by values: system home directory, organization, device IP, etc...
 */
void StdFunc::Init()
{
    s_systemHomeDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/"
        + QCoreApplication::applicationName() + "/";
    if ((!s_systemHomeDir.contains("/root")) && s_systemHomeDir.startsWith("//"))
    {
        if (s_systemHomeDir.front() == '/')
            s_systemHomeDir.replace(0, 1, "/root");
    }

    QDir dir(s_systemHomeDir);
    if (!dir.exists())
        dir.mkdir(s_systemHomeDir);
    auto sets = std::unique_ptr<QSettings>(new QSettings);
    SetOrganizationString(sets->value("OrganizationString", "Р&К").toString());
    SetDeviceIP(sets->value("DeviceIP", "172.16.11.12").toString());
    SetTuneRequestCount(sets->value("TuneRequestCount", "20").toInt());
}

/// \brief Converts a version from quint32 datatype to string view.
QString StdFunc::VerToStr(quint32 num)
{
    int mv = (num & 0xFF000000) >> 24;
    int lv = (num & 0x00FF0000) >> 16;
    int sv = (num & 0x0000FFFF);
    QString tmpString
        = QString::number(mv, 10) + "." + QString::number(lv, 10) + "-" + QString("%1").arg(sv, 4, 10, QChar('0'));
    return tmpString;
}

/// \brief Converts a version from string view to quint32 datatype.
quint32 StdFunc::StrToVer(const QString &str)
{
    auto dotPos = str.indexOf('.');
    auto dashPos = str.indexOf('-');
    // Don't trust clazy: Qt6 QString hasn't leftRef, midRef and rightRef methods
    int mv = str.left(dotPos).toInt() << 24;
    int lv = str.mid(dotPos + 1, dashPos - dotPos - 1).toInt() << 16;
    int sv = str.right(str.size() - dashPos - 1).toInt();
    return (mv | lv | sv);
}

/// \brief Converts a value from string view to float point datatype.
float StdFunc::ToFloat(const QString &text, bool *ok)
{
    bool floatok;
    float tmpf;
    tmpf = text.toFloat(&floatok);
    if (!floatok)
    {
        qCritical() << "Значение " << text << " не может быть переведено во float";
        if (ok != 0)
            *ok = false;
        return 0;
    }
    if (ok != 0)
        *ok = true;
    return tmpf;
}

QString StdFunc::toString(float value, int precision, bool exp)
{
    if (value >= MAXFLOAT || value <= -MAXFLOAT)
        return "***";
    else if (exp == true)
        return QString::number(value, 'e', precision);
    else
        return QString::number(value, 'f', precision);
}

/*! \brief Checks that the number is in the specified interval.
 *  \param var Checked number.
 *  \param base,tolerance Limits of specified interval.
 */
bool StdFunc::FloatIsWithinLimits(double var, double base, double tolerance)
{
    auto tmpf = fabs(var - base);
    return (tmpf < fabs(tolerance));
}

/// \brief Sets new path for home directory field.
void StdFunc::SetHomeDir(const QString &dir)
{
    s_homeDir = dir;
}

/// \brief Returns path for home directory.
QString StdFunc::GetHomeDir()
{
    return s_homeDir;
}

/// \brief Returns path for system home directory.
QString StdFunc::GetSystemHomeDir()
{
    return s_systemHomeDir;
}

QString StdFunc::configDir()
{
    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
}

QString StdFunc::dataDir()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

QString StdFunc::WhoAmI()
{
    QProcess process;

    process.setProgram("whoami");

    process.start();

    while (process.state() != QProcess::NotRunning)
        qApp->processEvents();

    return process.readAll();
}

/*! \brief Sets new device's IP.
 *  \param ip String that contains new IP address.
 */
void StdFunc::SetDeviceIP(const QString &ip)
{
    s_deviceIP = ip;
    auto sets = std::unique_ptr<QSettings>(new QSettings);
    sets->setValue("DeviceIP", ip);
}

/// \brief Returns device's IP.
QString StdFunc::ForDeviceIP()
{
    return s_deviceIP;
}

/*! \brief Sets new organization name.
 *  \param str New organization name in string view.
 */
void StdFunc::SetOrganizationString(const QString &str)
{
    s_OrganizationString = str;
    auto sets = std::unique_ptr<QSettings>(new QSettings);
    sets->setValue("OrganizationString", str);
}

/// \brief Returns organization name.
QString StdFunc::OrganizationString()
{
    return s_OrganizationString;
}

/// \brief Sets new tune request count.
void StdFunc::SetTuneRequestCount(int n)
{
    m_tuneRequestCount = n;
}

/// \brief Returns tune request count.
int StdFunc::TuneRequestCount()
{
    return m_tuneRequestCount;
}

/// \brief Sets cancel s_state when enabled.
void StdFunc::Cancel()
{
    if (s_state.cancelEnabled)
        s_state.cancelled = true;
}

/// \brief Turns off cancel s_state.
void StdFunc::ClearCancel()
{
    s_state.cancelled = false;
}

/// \brief Returns cancel s_state.
bool StdFunc::IsCancelled()
{
    return s_state.cancelled;
}

/// \brief Disallows to set cancel s_state.
void StdFunc::SetCancelDisabled()
{
    s_state.cancelEnabled = false;
}

/// \brief Allows to set cancel s_state.
void StdFunc::SetCancelEnabled()
{
    s_state.cancelEnabled = true;
}

/*! \brief Returns the position of first bit set.
 *  \details Returns position of the first '1' starting from LSB.
 *  \param dword 32bit bitstring.
 */
int StdFunc::IndexByBit(quint32 dword)
{
    quint32 bit = 0x00000001;
    for (int i = 0; i < 32; ++i)
        if (dword & bit)
            return (i + 1);
    return 0;
}

/*! \brief Returns the 32bit bitstring by index position.
 *  \details Returns 32bit bitstring with '1' in index position.
 *  \param index Position of '1' from LSB.
 *  \return Example: 0 => 0, 1 => 1, 2 => 2, 3 => 4, ...
 */
quint32 StdFunc::BitByIndex(int index)
{
    quint32 bit = 0x00000001;
    if ((index == 0) || (index > 31))
        return 0;
    return (bit << (index - 1));
}

/// \brief Puts the thread to sleep for a given time in ms.
void StdFunc::Wait(int ms)
{
    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(ms);
    loop.exec();

    /*
    QElapsedTimer tmr;
    tmr.start();
    while (tmr.elapsed() < ms)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        QThread::msleep(MAINSLEEP);
    }
    */
}

/*! \brief Ping IP address, return IP address if host is alive or return 0 if host is dead
 *  \details Platform dependent ping function, ping IP address through cmdline utility,
 *  parse cmd output. If output contains TTL host is alive else host is dead.
 *  \return IP address if host is alive, otherwise returns 0.
 */
quint32 StdFunc::Ping(quint32 addr)
{
    QString exec = "ping";
#ifdef Q_OS_WINDOWS
    QString param = "-n";
#endif
#ifdef Q_OS_LINUX
    QString param = "-c";
#endif
    QHostAddress host(addr);
    auto pingProcess = new QProcess;
    QStringList params { param, "1", host.toString() };

    pingProcess->start(exec, params, QIODevice::ReadOnly);
    if (pingProcess->waitForFinished(100))
    {
        QString p_stdout = pingProcess->readAllStandardOutput();
        QString p_stderr = pingProcess->readAllStandardError();
        QStringList list = p_stderr.isEmpty() ? p_stdout.split("\r\n") : p_stderr.split("\r\n");
        if (std::any_of(list.constBegin(), list.constEnd(),
                [](const QString &i) { return i.contains("TTL", Qt::CaseInsensitive); }))
        {
            delete pingProcess;
            return addr;
        }
    }
    pingProcess->kill();
    pingProcess->waitForFinished();
    delete pingProcess;
    return 0;
}

/*! \brief Checks port and IPv4 address for connection.
 *  \details Checks if the connection can be made with given IP address and port.
 *  \param ip4Addr[in] IPv4 host address.
 *  \param port[in] Connection port.
 *  \return IPv4 address if connection can be made, 0 otherwise.
 */
quint32 StdFunc::CheckPort(quint32 ip4Addr, quint16 port)
{
    QHostAddress host(ip4Addr);
    QTcpSocket *sock = new QTcpSocket;
    QTimer *timer = new QTimer;
    timer->setInterval(1000);
    QEventLoop *loop = new QEventLoop;

    QObject::connect(sock, &QAbstractSocket::connected, [&]() { loop->quit(); });
    QObject::connect(timer, &QTimer::timeout, [&]() {
        loop->quit();
        ip4Addr = 0;
    });
    timer->start();
    // qDebug() << "Timer started";
    Q_ASSERT(sock != nullptr);
    sock->connectToHost(host, port);
    loop->exec(QEventLoop::ExcludeUserInputEvents);
    sock->disconnect();
    sock->disconnectFromHost();
    delete sock;
    timer->stop();
    delete timer;
    delete loop;
    qDebug() << (ip4Addr ? "Port opened" : "Port closed");
    return ip4Addr;
}

/*! \brief Removes specified substring from specified string.
 *  \param str[in, out] The string from which the substring will be removed.
 *  \param substr[i] The substring that will be removed from string.
 */
void StdFunc::RemoveSubstr(std::string &str, std::string &substr)
{
    std::string::size_type n = substr.length();
    for (std::string::size_type i = str.find(substr); i != std::string::npos; i = str.find(substr))
        str.erase(i, n);
}
