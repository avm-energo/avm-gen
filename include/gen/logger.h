#pragma once

#include <QLoggingCategory>
#include <QtCore>
#include <gen/gen_export.h>

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

    const QMap<Logger::MessageTypes, MsgDescr> c_msgTypes {
        { Logger::Debug, { Logger::LogLevels::LOGLEVEL_DEBUG, "[DEBUG]" } },      //
        { Logger::Info, { Logger::LogLevels::LOGLEVEL_INFO, "[INFO]" } },         //
        { Logger::Warning, { Logger::LogLevels::LOGLEVEL_WARN, "[WARNING]" } },   //
        { Logger::Critical, { Logger::LogLevels::LOGLEVEL_CRIT, "[CRITICAL]" } }, //
        { Logger::Fatal, { Logger::LogLevels::LOGLEVEL_FATAL, "[FATAL]" } },      //
        { Logger::All, { Logger::LogLevels::LOGLEVEL_FATAL, "" } }                //
    };

    const QMap<QString, Logger::LogLevels> c_logLevelsMap = { { "Debug", Logger::LogLevels::LOGLEVEL_DEBUG },
        { "Info", Logger::LogLevels::LOGLEVEL_INFO }, { "Fatal", Logger::LogLevels::LOGLEVEL_FATAL },
        { "Warn", Logger::LogLevels::LOGLEVEL_WARN }, { "Error", Logger::LogLevels::LOGLEVEL_CRIT } };

    Logger();

    void writeLog(MessageTypes type, const QString &msg);
    void writeStart(const QString &filename);
    void setLogLevel(LogLevels level);
    void setLogLevel(const QString &level);
    QStringList logLevelsList();

protected:
private:
    LogLevels m_logLevel;
    QString m_logFilename;
    QMutex m_mutex;
};
