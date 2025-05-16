#include <QDir>
#include <QStandardPaths>
#include <gen/settings.h>

QString Settings::s_workDir = "";
QString Settings::s_oldGroup = "";
QSettings Settings::s_settings = QSettings();

Settings::Settings()
{
    for (QString dirstr : { configDir(), dataDir(), logDir() })
    {
        QDir dir(dirstr);
        if (!dir.exists())
            dir.mkdir(dirstr);
    }
    s_workDir = QString(get("workDir", dataDir()));
}

void Settings::pushGroup(const QString &newGroup)
{
    s_oldGroup = s_settings.group();
    s_settings.endGroup();
    s_settings.beginGroup(newGroup);
}

void Settings::popGroup()
{
    s_settings.endGroup();
    s_settings.beginGroup(s_oldGroup);
}

QStringList Settings::groups(const QString &key)
{
    QStringList sl;
    if (!key.isEmpty())
        pushGroup(key);
    sl.append(s_settings.childGroups());
    if (!key.isEmpty())
        popGroup();
    return sl;
}

utils::Convertable Settings::get(const QString &key, const QVariant &defValue)
{
    return utils::Convertable { s_settings.value(key, defValue) };
}

void Settings::set(const QString &key, const QVariant &value)
{
    s_settings.setValue(key, value);
}

void Settings::remove(const QString &name)
{
    s_settings.remove(name);
}

bool Settings::groupExist(const QString &name)
{
    return s_settings.contains(name);
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
    return s_workDir;
}

void Settings::setWorkDir(const QString &dir)
{
    s_workDir = dir;
    set("workDir", dir);
}

QString Settings::logDir()
{
    return dataDir() + "logs/";
}
