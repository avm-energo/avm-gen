#include <QDir>
#include <QStandardPaths>
#include <gen/settings.h>

QString Settings::m_workDir = "";

Settings::Settings()
{
    for (QString dirstr : { configDir(), dataDir(), logDir() })
    {
        QDir dir(dirstr);
        if (!dir.exists())
            dir.mkdir(dirstr);
    }
    m_workDir = value("workDir", dataDir()).toString();
}

QVariant Settings::value(const QString &key, const QVariant &defValue)
{
    QSettings sets;
    return sets.value(key, defValue);
}

void Settings::setValue(const QString &key, const QVariant &value)
{
    QSettings sets;
    sets.setValue(key, value);
}

QString Settings::configDir()
{
    QString configDir;
#ifdef Q_OS_WINDOWS
    configDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#else
#ifdef Q_OS_LINUX
    configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
#endif
#endif
    return configDir;
}

QString Settings::dataDir()
{
    QString dataDir;
    dataDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    return dataDir;
}

QString Settings::workDir()
{
    return m_workDir;
}

void Settings::setWorkDir(const QString &dir)
{
    m_workDir = dir;
    setValue("workDir", dir);
}

QString Settings::logDir()
{
    return dataDir() + "logs/";
}
