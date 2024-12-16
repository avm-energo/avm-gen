#pragma once

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QTimeZone>
#include <gen/gen_export.h>

#define SLEEPINT 20
#define TIMEOUT_MAIN 2000
#define TIMEOUT_BIG 30000

class GENLIB_EXPORT TimeFunc
{
public:
    TimeFunc();

    static inline void Wait(int ms = SLEEPINT)
    {
        QElapsedTimer tmr;
        tmr.start();
        while (tmr.elapsed() < ms)
            QCoreApplication::processEvents(QEventLoop::AllEvents);
    }

    static QString NsTimeToString(quint64 nstime);
    static QString UnixTime64ToString(quint64 utime, QTimeZone tz = QTimeZone::utc());
    static QString UnixTime64ToInvStringFractional(quint64 utime, QTimeZone tz = QTimeZone::utc());
    static QString UnixTime32ToInvString(quint32 utime, QTimeZone tz = QTimeZone::utc());
    static quint64 InvStringToUnixTime32(QString utime, QTimeZone tz = QTimeZone::utc());
    static QTimeZone userTimeZone();
    static QString userTimeZoneName()
    {
        return userTimeZone().displayName(QTimeZone::StandardTime, QTimeZone::OffsetName);
    }
    static bool WaitFor(bool &flag, int timeoutms = TIMEOUT_MAIN);

    /// \brief get current system timezone in int from -12 to +14
    static int curTimeZone();
};
