#include <QDateTime>
#include <QEventLoop>
#include <avm-gen/settings.h>
#include <avm-gen/timefunc.h>

TimeFunc::TimeFunc()
{
}

bool TimeFunc::WaitFor(bool &flag, int timeoutms)
{
    if (flag)
        return true;

    QEventLoop loop;

    // Poll timer: checks the flag every SLEEPINT ms and quits when it's set.
    QTimer pollTimer;
    pollTimer.setInterval(SLEEPINT);
    QObject::connect(&pollTimer, &QTimer::timeout, [&]() {
        if (flag)
            loop.quit();
    });

    // Deadline timer: exits the loop unconditionally after timeoutms.
    QTimer deadlineTimer;
    deadlineTimer.setSingleShot(true);
    QObject::connect(&deadlineTimer, &QTimer::timeout, &loop, &QEventLoop::quit);

    pollTimer.start();
    deadlineTimer.start(timeoutms);
    loop.exec();

    return flag;
}

int TimeFunc::curTimeZone()
{
    QDateTime datetime = QDateTime::currentDateTime();
#if QT_VERSION < QT_VERSION_CHECK(6, 8, 2)
    datetime.setTimeSpec(Qt::LocalTime);
#else
    datetime.setTimeZone(QTimeZone::LocalTime);
#endif
    return (datetime.offsetFromUtc() / 3600);
}

QString TimeFunc::NsTimeToString(quint64 nstime)
{
    quint32 unixtime = nstime / 1000000000ULL;
    QString tmps = QDateTime::fromSecsSinceEpoch(unixtime).toString("dd-MM-yyyy hh:mm:ss");
    quint32 nst = nstime % 1000000000ULL;                                   // sub-second remainder in nanoseconds
    tmps += "." + QString("%1").arg((nst / 1000000ULL), 3, 10, QChar('0')); // milliseconds
    nst = nst % 1000000ULL;
    tmps += "." + QString("%1").arg((nst / 1000ULL), 3, 10, QChar('0')); // microseconds
    nst = nst % 1000ULL;
    tmps += "." + QString("%1").arg(nst, 3, 10, QChar('0')); // nanoseconds
    return tmps;
}

QString TimeFunc::UnixTime64ToString(quint64 utime, QTimeZone tz)
{
    quint32 tmpi = utime >> 32;
    QDateTime tn = QDateTime::fromSecsSinceEpoch(tmpi, tz);
    quint32 timens = utime & 0xFFFFFFFF;
    QString ms = QString::number((timens / 1000000));
    QString mcs = QString::number(timens / 1000 - ms.toULongLong() * 1000);
    QString ns = QString::number(timens - ms.toULongLong() * 1000000 - mcs.toULongLong() * 1000);
    QString time = tn.toString("dd/MM/yyyy hh:mm:ss.") + ms + "." + mcs + "." + ns;
    return time;
}

QString TimeFunc::UnixTime64ToInvStringFractional(quint64 utime, QTimeZone tz)
{
    quint32 tmpi = utime >> 32;
    quint32 subSeconds = utime & 0x00000000FFFFFFFF;
    QDateTime tn = QDateTime::fromSecsSinceEpoch(tmpi, tz);
    QString outs = tn.toString("yyyy/MM/dd hh:mm:ss");
    QString frac = QString::number(subSeconds / 0x418937); // 0x418937 = 2^32 / 1000
    frac = frac.rightJustified(3, '0');
    outs += "." + frac;
    return outs;
}

QString TimeFunc::UnixTime32ToInvString(quint32 utime, QTimeZone tz)
{
    QDateTime tn = QDateTime::fromSecsSinceEpoch(utime, tz);
    return tn.toString("yyyy/MM/dd hh:mm:ss");
}

quint64 TimeFunc::InvStringToUnixTime32(const QString &utime, QTimeZone tz)
{
    Q_UNUSED(tz)
    return QDateTime::fromString(utime, "yyyy/MM/dd hh:mm:ss").toSecsSinceEpoch();
}

QTimeZone TimeFunc::userTimeZone()
{
    QString timezone = QTimeZone::systemTimeZone().displayName(QTimeZone::StandardTime, QTimeZone::OffsetName);
    timezone = QString(Settings::get("timeZone", timezone));
    if (!timezone.isEmpty())
        return QTimeZone(timezone.toUtf8());
    return QTimeZone::systemTimeZone();
}
