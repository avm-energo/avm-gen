#pragma once

#include <QLoggingCategory>
#include <QMutex>
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

    Logger();

    void writeLog(MessageTypes type, const QString &msg);
    void writeStart(const QString &filename);
    void setLogLevel(LogLevels level);
    void setLogLevel(const QString &level);
    static QStringList logLevelsList();

    static const QMap<QString, Logger::LogLevels> s_logLevelsMap;

protected:
private:
    LogLevels m_logLevel;
    QString m_logFilename;
    QMutex m_mutex;
};
