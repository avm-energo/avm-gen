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

    // static const QMap<QString, Logger::LogLevels> _logLevelsMap;

    void static messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    void static messageHandlerWithErrorQueue(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    void static writeLog(MessageTypes type, const QString &msg);
    void static writeStart(const QString &filename);
    void static setLogLevel(LogLevels level);
    void static setLogLevel(const QString &level);
    QStringList static logLevelsList();

protected:
    Logger() = delete;

private:
    static LogLevels _logLevel;
    static QMutex _mutex;
};
