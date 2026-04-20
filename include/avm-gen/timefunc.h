#pragma once

#include <QCoreApplication>
#include <QTimeZone>
#include <QTimer>
#include <avm-gen/gen_export.h>

#define SLEEPINT 20
#define TIMEOUT_MAIN 2000
#define TIMEOUT_BIG 30000

class GENLIB_EXPORT TimeFunc
{
public:
    /// \brief Default constructor — no initialisation needed; all methods are stateless.
    TimeFunc();

    static inline void Wait(int ms = SLEEPINT)
    {
        QEventLoop loop;
        QTimer tmr;
        tmr.start();
        tmr.setSingleShot(true);
        QObject::connect(&tmr, &QTimer::timeout, &loop, &QEventLoop::quit);
        tmr.start(ms);
        loop.exec();
    }

    /*! \brief Converts a nanosecond-resolution timestamp to a human-readable string.
     *  \param nstime  Absolute time in nanoseconds since Unix epoch.
     *  \return String in the format "dd-MM-yyyy hh:mm:ss.mmm.µµµ.nnn".
     */
    static QString NsTimeToString(quint64 nstime);

    /*! \brief Converts a 64-bit packed timestamp (upper 32 bits = seconds, lower 32 bits = sub-seconds in ns)
     *         to a string "dd/MM/yyyy hh:mm:ss.ms.µs.ns".
     *  \param utime  Packed 64-bit timestamp.
     *  \param tz     Target timezone for the calendar representation.
     */
    static QString UnixTime64ToString(quint64 utime, QTimeZone tz = QTimeZone::utc());

    /*! \brief Converts a 64-bit packed timestamp to an ISO-like string with millisecond fraction.
     *  \param utime  Packed 64-bit timestamp (upper 32 bits = seconds, lower 32 bits = sub-seconds).
     *  \param tz     Target timezone.
     *  \return String in the format "yyyy/MM/dd hh:mm:ss.mmm".
     */
    static QString UnixTime64ToInvStringFractional(quint64 utime, QTimeZone tz = QTimeZone::utc());

    /*! \brief Converts a 32-bit Unix timestamp to an ISO-like string without sub-second precision.
     *  \param utime  Seconds since Unix epoch.
     *  \param tz     Target timezone.
     *  \return String in the format "yyyy/MM/dd hh:mm:ss".
     */
    static QString UnixTime32ToInvString(quint32 utime, QTimeZone tz = QTimeZone::utc());

    /*! \brief Parses an ISO-like date-time string into a Unix timestamp.
     *  \param utime  String in the format "yyyy/MM/dd hh:mm:ss".
     *  \param tz     Reserved for future use (currently ignored).
     *  \return Seconds since Unix epoch (local time assumed by Qt's fromString()).
     */
    static quint64 InvStringToUnixTime32(const QString &utime, QTimeZone tz = QTimeZone::utc());

    /*! \brief Returns the user-configured timezone, falling back to the system timezone.
     *  \details Reads the "timeZone" key from persistent settings.  If the stored value
     *           is invalid or absent the system timezone is returned unchanged.
     */
    static QTimeZone userTimeZone();

    static QString userTimeZoneName()
    {
        return userTimeZone().displayName(QTimeZone::StandardTime, QTimeZone::OffsetName);
    }

    /*! \brief Waits until \a flag becomes true or \a timeoutms elapses.
     *  \param flag       Reference to the boolean flag to wait on.
     *  \param timeoutms  Maximum wait time in milliseconds.
     *  \return true if \a flag was set before the timeout, false otherwise.
     */
    static bool WaitFor(bool &flag, int timeoutms = TIMEOUT_MAIN);

    /*! \brief Returns the local timezone offset from UTC in whole hours.
     *  \return Positive values east of UTC, negative west (e.g. +3 for Moscow, -5 for EST).
     */
    static int curTimeZone();
};
