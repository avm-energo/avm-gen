#pragma once

#include <QByteArray>
#include <QFile>
#include <QMutex>
#include <gen/gen_export.h>

#define LOG_MAX_SIZE 1048576

namespace
{
constexpr char logStart[] = "=== Log started ===";
constexpr char logExt[] = "log";
}

enum class [[deprecated]] LogLevel : std::uint8_t
{
    Info = 0,
    Warning,
    Error,
    Debug,
    Undefined = std::numeric_limits<std::uint8_t>::max()
};

class GENLIB_EXPORT [[deprecated]] LogClass
{
private:
    bool m_canLog;
    QFile m_file;
    QMutex m_mutex;

public:
    explicit LogClass() noexcept;
    ~LogClass() noexcept;

    void init(const QString &filename);
    QString getFilename() const noexcept;

    void info(const QString &str);
    void warning(const QString &str);
    void error(const QString &str);
    void debug(const QString &str);
    void logging(const QString &message, const LogLevel level = LogLevel::Info);
    void writeFile(const QString &msg, const QString &prepend = "");
};
