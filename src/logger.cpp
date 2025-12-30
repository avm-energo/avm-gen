#include <avm-gen/files.h>
#include <avm-gen/logger.h>
#include <avm-gen/settings.h>
#include <avm-gen/stdfunc.h>

static QString s_messageHandlerLogFileName = "logger.log";

const QMap<QString, Logger::LogLevels> Logger::s_logLevelsMap = { { "Debug", Logger::LogLevels::LOGLEVEL_DEBUG },
    { "Info", Logger::LogLevels::LOGLEVEL_INFO }, { "Fatal", Logger::LogLevels::LOGLEVEL_FATAL },
    { "Warn", Logger::LogLevels::LOGLEVEL_WARN }, { "Error", Logger::LogLevels::LOGLEVEL_CRIT } };

const QMap<Logger::MessageTypes, Logger::MsgDescr> c_msgTypes {
    { Logger::Debug, { Logger::LogLevels::LOGLEVEL_DEBUG, "[DEBUG]" } },      //
    { Logger::Info, { Logger::LogLevels::LOGLEVEL_INFO, "[INFO]" } },         //
    { Logger::Warning, { Logger::LogLevels::LOGLEVEL_WARN, "[WARNING]" } },   //
    { Logger::Critical, { Logger::LogLevels::LOGLEVEL_CRIT, "[CRITICAL]" } }, //
    { Logger::Fatal, { Logger::LogLevels::LOGLEVEL_FATAL, "[FATAL]" } },      //
    { Logger::All, { Logger::LogLevels::LOGLEVEL_FATAL, "" } }                //
};

Logger::Logger()
{
    m_logLevel = Logger::LogLevels::LOGLEVEL_WARN;
    m_logFilename = "logger.log";
    m_mutex = new QMutex;
}

void Logger::writeLog(Logger::MessageTypes type, const QString &msg)
{
    QFile logFile;
    QTextStream out;

    QMutexLocker locker(m_mutex);
    if (m_logLevel < c_msgTypes.value(type).loglevel)
        return;
    logFile.setFileName(m_logFilename);
    out.setDevice(&logFile);
    if (logFile.open(QFile::ReadWrite | QFile::Text | QFile::Append))
    {
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz "); // Log datetime
        out << c_msgTypes.value(type).prefix << msg << "\n";
        out.flush(); // Flush buffer
        Files::checkNGzip(&logFile);
        logFile.close();
    }
}

void Logger::writeStart(const QString &filename)
{
    QMutexLocker locker(m_mutex);
    m_logFilename = Settings::logDir() + filename;
    QFile logFile(m_logFilename);
    Files::makePath(logFile);
    QTextStream out;
    out.setDevice(&logFile);
    if (logFile.open(QFile::ReadWrite | QFile::Text | QFile::Append))
    {
        out << "=====================================\nLog file started at "
            << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") + "\n"
            << QCoreApplication::applicationName() << " v." << QCoreApplication::applicationVersion();
        out.flush();
        Files::checkNGzip(&logFile);
        logFile.close();
    }
}

void Logger::setLogLevel(LogLevels level)
{
    m_logLevel = level;
}

void Logger::setLogLevel(const QString &level)
{
    m_logLevel = s_logLevelsMap.value(level);
}

Logger::MessageTypes Logger::qtMessageTypeToMessageType(QtMsgType type)
{
    const QMap<QtMsgType, Logger::MessageTypes> c_msgTypesTranslateMap { { QtDebugMsg, Logger::Debug },
        { QtInfoMsg, Logger::Info }, { QtWarningMsg, Logger::Warning }, { QtCriticalMsg, Logger::Critical },
        { QtFatalMsg, Logger::Fatal } };
    return c_msgTypesTranslateMap[type];
}

Logger::LogLevels Logger::qtMessageTypeToLoglevel(QtMsgType type)
{
    const QMap<QtMsgType, Logger::LogLevels> c_msgTypesTranslateMap { { QtDebugMsg, LOGLEVEL_DEBUG },
        { QtInfoMsg, LOGLEVEL_INFO }, { QtWarningMsg, LOGLEVEL_WARN }, { QtCriticalMsg, LOGLEVEL_CRIT },
        { QtFatalMsg, LOGLEVEL_FATAL } };
    return c_msgTypesTranslateMap[type];
}

QStringList Logger::logLevelsList()
{
    return s_logLevelsMap.keys();
}

/// Категории мы сейчас не используем, задел на будущее
Q_LOGGING_CATEGORY(logDebug, "Debug")
Q_LOGGING_CATEGORY(logInfo, "Info")
Q_LOGGING_CATEGORY(logWarning, "Warning")
Q_LOGGING_CATEGORY(logCritical, "Critical")
Q_LOGGING_CATEGORY(logFatal, "Fatal")
