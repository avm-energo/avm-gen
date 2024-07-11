#include "gen/logger.h"

#include <gen/errorqueue.h>
#include <gen/files.h>
#include <gen/stdfunc.h>
#include <iostream>

struct MsgDescr
{
    Logger::LogLevels loglevel;
    const char *prefix;
};

const static QMap<Logger::MessageTypes, MsgDescr> _msgTypes {
    { Logger::Debug, { Logger::LogLevels::LOGLEVEL_DEBUG, "[DEBUG]" } },      //
    { Logger::Info, { Logger::LogLevels::LOGLEVEL_INFO, "[INFO]" } },         //
    { Logger::Warning, { Logger::LogLevels::LOGLEVEL_WARN, "[WARNING]" } },   //
    { Logger::Critical, { Logger::LogLevels::LOGLEVEL_CRIT, "[CRITICAL]" } }, //
    { Logger::Fatal, { Logger::LogLevels::LOGLEVEL_FATAL, "[FATAL]" } },      //
    { Logger::All, { Logger::LogLevels::LOGLEVEL_FATAL, "" } }                //
};

const static QMap<QtMsgType, Logger::MessageTypes> _msgTypesTranslateMap { { QtDebugMsg, Logger::Debug },
    { QtInfoMsg, Logger::Info }, { QtWarningMsg, Logger::Warning }, { QtCriticalMsg, Logger::Critical },
    { QtFatalMsg, Logger::Fatal } };

const QMap<QString, Logger::LogLevels> _logLevelsMap = { { "Debug", Logger::LogLevels::LOGLEVEL_DEBUG },
    { "Info", Logger::LogLevels::LOGLEVEL_INFO }, { "Fatal", Logger::LogLevels::LOGLEVEL_FATAL },
    { "Warn", Logger::LogLevels::LOGLEVEL_WARN }, { "Error", Logger::LogLevels::LOGLEVEL_CRIT } };

static QString s_logFilename = "logger.log"; // имя по умолчанию
Logger::LogLevels Logger::_logLevel = Logger::LogLevels::LOGLEVEL_WARN;
QMutex Logger::_mutex;

void Logger::messageHandlerWithErrorQueue(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Logger::MessageTypes msgType = _msgTypesTranslateMap[type];
    if (_logLevel < _msgTypes.value(msgType).loglevel)
        return;
    QMutexLocker locker(&_mutex);
    QStringList buffer = QString(context.file).split("\\");
    QString sourceFile = buffer.isEmpty() ? "" : buffer.takeLast();

    std::string function = context.function ? context.function : "";
    std::string rubbish(" __cdecl");
    StdFunc::RemoveSubstr(function, rubbish);

    ErrorMsg tmpm {
        QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss"), // DateTime
        type,                                                         // Msg type
        sourceFile,                                                   // File
        context.line,                                                 // Line
        msg                                                           // Message
    };
    ErrorQueue::GetInstance().pushError(tmpm);
    writeLog(msgType, msg);
}

void Logger::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)
    Logger::MessageTypes msgType = _msgTypesTranslateMap[type];
    if (_logLevel < _msgTypes.value(msgType).loglevel)
        return;
    QMutexLocker locker(&_mutex);
    writeLog(msgType, msg);
}

void Logger::writeLog(Logger::MessageTypes type, const QString &msg)
{
    QFile logFile;
    QTextStream out;
    logFile.setFileName(s_logFilename);
    out.setDevice(&logFile);
    if (logFile.open(QFile::ReadWrite | QFile::Text | QFile::Append))
    {
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz "); // Log datetime
        out << _msgTypes.value(type).prefix << msg << "\n";
        out.flush(); // Flush buffer
        Files::checkNGzip(&logFile);
        logFile.close();
    }
}

void Logger::writeStart(const QString &filename)
{
    QMutexLocker locker(&_mutex);
    s_logFilename = filename;
    QFile logFile(s_logFilename);
    Files::makePath(logFile);
    QTextStream out;
    out.setDevice(&logFile);
    logFile.open(QFile::ReadWrite | QFile::Text | QFile::Append);
    out << "=====================================\nLog file started at "
        << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") + "\n"
        << QCoreApplication::applicationName() << " v." << QCoreApplication::applicationVersion();
    out.flush();
    Files::checkNGzip(&logFile);
    logFile.close();
}

void Logger::setLogLevel(LogLevels level)
{
    _logLevel = level;
}

void Logger::setLogLevel(const QString &level)
{
    _logLevel = _logLevelsMap.value(level);
}

/// Категории мы сейчас не используем, задел на будущее
Q_LOGGING_CATEGORY(logDebug, "Debug")
Q_LOGGING_CATEGORY(logInfo, "Info")
Q_LOGGING_CATEGORY(logWarning, "Warning")
Q_LOGGING_CATEGORY(logCritical, "Critical")
Q_LOGGING_CATEGORY(logFatal, "Fatal")
