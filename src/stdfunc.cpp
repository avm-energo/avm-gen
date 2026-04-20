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
#include <avm-gen/error.h>
#include <avm-gen/pch.h>
#include <avm-gen/stdfunc.h>

decltype(StdFunc::s_state) StdFunc::s_state {};

void StdFunc::Init()
{
}

QString StdFunc::VerToStr(quint32 num)
{
    int mv = (num & 0xFF000000) >> 24;
    int lv = (num & 0x00FF0000) >> 16;
    int sv = (num & 0x0000FFFF);
    QString tmpString
        = QString::number(mv, 10) + "." + QString::number(lv, 10) + "-" + QString("%1").arg(sv, 4, 10, QChar('0'));
    return tmpString;
}

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

float StdFunc::ToFloat(const QString &text, bool &ok)
{
    float tmpf;
    tmpf = text.toFloat(&ok);
    if (!ok)
    {
        qCritical() << "Значение " << text << " не может быть переведено во float";
        return 0;
    }
    return tmpf;
}

QString StdFunc::toString(float value, int precision, bool exp)
{
    if (value >= MAXFLOAT || value <= -MAXFLOAT)
        return "***";
    else if (exp)
        return QString::number(value, 'e', precision);
    else
        return QString::number(value, 'f', precision);
}

bool StdFunc::FloatIsWithinLimits(double var, double base, double tolerance)
{
    auto tmpf = fabs(var - base);
    return (tmpf < fabs(tolerance));
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

void StdFunc::Cancel()
{
    if (s_state.cancelEnabled)
        s_state.cancelled = true;
}

void StdFunc::ClearCancel()
{
    s_state.cancelled = false;
}

bool StdFunc::IsCancelled()
{
    return s_state.cancelled;
}

void StdFunc::SetCancelDisabled()
{
    s_state.cancelEnabled = false;
}

void StdFunc::SetCancelEnabled()
{
    s_state.cancelEnabled = true;
}

int StdFunc::IndexByBit(quint32 dword)
{
    for (int i = 0; i < 32; ++i)
        if (dword & (quint32(1) << i))
            return (i + 1);
    return 0;
}

quint32 StdFunc::BitByIndex(int index)
{
    quint32 bit = 0x00000001;
    if ((index == 0) || (index > 31))
        return 0;
    return (bit << (index - 1));
}

void StdFunc::Wait(int ms)
{
    QTimer timer;
    timer.setSingleShot(true);
    QEventLoop loop;
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(ms);
    loop.exec();
}

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

void StdFunc::RemoveSubstr(std::string &str, std::string &substr)
{
    std::string::size_type n = substr.length();
    for (std::string::size_type i = str.find(substr); i != std::string::npos; i = str.find(substr))
        str.erase(i, n);
}
