#pragma once

#include <QLoggingCategory>
#include <QMutex>
#include <QtCore>
#include <avm-gen/gen_export.h>

Q_DECLARE_LOGGING_CATEGORY(logDebug)
Q_DECLARE_LOGGING_CATEGORY(logInfo)
Q_DECLARE_LOGGING_CATEGORY(logWarning)
Q_DECLARE_LOGGING_CATEGORY(logCritical)
Q_DECLARE_LOGGING_CATEGORY(logFatal)

class GENLIB_EXPORT Logger
{
public:
    enum LogLevels
    {
        LOGLEVEL_DEBUG = 4,
        LOGLEVEL_INFO = 3,
        LOGLEVEL_WARN = 2,
        LOGLEVEL_CRIT = 1,
        LOGLEVEL_FATAL = 0
    };

    enum MessageTypes
    {
        Debug,
        Info,
        Warning,
        Critical,
        Fatal,
        All
    };

    struct MsgDescr
    {
        Logger::LogLevels loglevel;
        const char *prefix;
    };

    /// \brief Constructs Logger with default WARN log level and "logger.log" filename.
    Logger();

    /*! \brief Writes a message to the log file if its level passes the current threshold.
     *  \param type  Message type (Debug, Info, Warning, Critical, Fatal).
     *  \param msg   The text to write.
     *  \details Thread-safe via QMutexLocker. After writing the line the function
     *           calls Files::checkNGzip() to compress and rotate the log when it
     *           exceeds the size limit.
     */
    void writeLog(MessageTypes type, const QString &msg);

    /*! \brief Opens the log file, writes a start banner, and stores the path.
     *  \param filename  Base filename (without directory); placed under Settings::logDir().
     *  \details Called once at application startup to mark a new session in the log.
     *           Thread-safe via QMutexLocker.
     */
    void writeStart(const QString &filename);

    /*! \brief Opens the log file, writes a start banner, and stores the path.
     *  \param filename  Base filename (with directory).
     *  \details Called once at application startup to mark a new session in the log.
     *           Thread-safe via QMutexLocker.
     */
    void writeRawStart(const QString &filename);

    /// \brief Sets the minimum log level by enum value.
    void setLogLevel(LogLevels level);

    /*! \brief Sets the minimum log level by string name.
     *  \param level One of: "Debug", "Info", "Warn", "Error", "Fatal".
     *  \details Falls back to LOGLEVEL_DEBUG (0) if the string is not recognised;
     *           prefer the enum overload when the value is known at compile time.
     */
    void setLogLevel(const QString &level);

    /*! \brief Converts a Qt message type to the library's MessageTypes enum.
     *  \param type  QtMsgType value produced by Qt's message system.
     *  \return Corresponding Logger::MessageTypes value.
     */
    static LogLevels qtMessageTypeToLoglevel(QtMsgType type);

    /*! \brief Converts a Qt message type to the library's LogLevels enum.
     *  \param type  QtMsgType value produced by Qt's message system.
     *  \return Corresponding Logger::LogLevels value.
     */
    static MessageTypes qtMessageTypeToMessageType(QtMsgType type);

    /// \brief Returns the list of valid log-level name strings accepted by setLogLevel(QString).
    static QStringList logLevelsList();

    static const QMap<QString, Logger::LogLevels> s_logLevelsMap;

protected:
private:
    LogLevels m_logLevel;
    QString m_logFilename;
    QMutex *m_mutex;

    void writeStart();
};
