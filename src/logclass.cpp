#include "gen/logclass.h"

#include <QDataStream>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QMutexLocker>
#include <gen/files.h>
#include <gen/stdfunc.h>

LogClass::LogClass() noexcept : m_canLog(false)
{
}

LogClass::~LogClass() noexcept
{
    m_file.flush();
    m_file.close();
}

void LogClass::init(const QString &filename)
{
    // тестовая проверка открытия файла на запись
    m_file.setFileName(StdFunc::dataDir() + filename);
    if (!m_file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append))
    {
        m_canLog = false;
        qCritical("Ошибка открытия файла");
    }
    else
    {
        m_canLog = true;
        info(::logStart);
        // writeRaw(::logStart);
    }
}

QString LogClass::getFilename() const noexcept
{
    return m_file.fileName();
}

void LogClass::info(const QString &str)
{
    if (m_canLog)
        writeFile(str, "Info");
}

void LogClass::warning(const QString &str)
{
    if (m_canLog)
        writeFile(str, "Warning");
}

void LogClass::error(const QString &str)
{
    if (m_canLog)
        writeFile(str, "Error");
}

void LogClass::debug(const QString &str)
{
    if (m_canLog)
        writeFile(str, "Debug");
}

void LogClass::logging(const QString &message, const LogLevel level)
{
    switch (level)
    {
    case LogLevel::Info:
        info(message);
        break;
    case LogLevel::Warning:
        warning(message);
        break;
    case LogLevel::Error:
        error(message);
        break;
    case LogLevel::Debug:
        debug(message);
        break;
    default:
        qWarning("Undefined logging level");
        break;
    }
}

void LogClass::writeFile(const QString &msg, const QString &prepend)
{
    QMutexLocker locker(&m_mutex);
    if (m_file.isOpen())
    {
        QString log = "[" + QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss.zzz") + "] ";
        if (!prepend.isEmpty())
            log = log + "[" + prepend + "] ";
        log = log + msg + "\n";
        m_file.write(log.toLocal8Bit());
        m_file.flush();
        Files::checkNGzip(&m_file);
    }
}
