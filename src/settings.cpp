#include <QDir>
#include <QStandardPaths>
#include <gen/settings.h>

QString Settings::s_workDir = "";
QString Settings::s_oldGroup = "";
#ifdef Q_OS_WINDOWS
constexpr char c_dirDelimiter[] = "\\";
#else
#ifdef Q_OS_LINUX
constexpr char c_dirDelimiter[] = "/";
#endif
#endif

Settings::Settings()
{
}

void Settings::pushGroup(const QString &newGroup)
{
    // s_oldGroup = instance().conf.group();
    // instance().conf.endGroup();
    instance().conf.beginGroup(newGroup);
}

void Settings::popGroup()
{
    instance().conf.endGroup();
    // instance().conf.beginGroup(s_oldGroup);
}

QStringList Settings::groups(const QString &key)
{
    QStringList sl;
    if (!key.isEmpty())
        pushGroup(key);
    sl.append(instance().conf.childGroups());
    if (!key.isEmpty())
        popGroup();
    return sl;
}

utils::Convertable Settings::get(const QString &key, const QVariant &defValue)
{
    QString oldGroup = instance().conf.group();
    utils::Convertable Value = utils::Convertable { instance().conf.value(key, defValue) };
    return utils::Convertable { instance().conf.value(key, defValue) };
}

void Settings::set(const QString &key, const QVariant &value)
{
    instance().conf.setValue(key, value);
}

void Settings::remove(const QString &name)
{
    instance().conf.remove(name);
}

bool Settings::groupExist(const QString &name)
{
    return instance().conf.contains(name);
}

QString Settings::configDir()
{
    QString configDir;
#ifdef Q_OS_WINDOWS
    configDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + c_dirDelimiter;
#else
#ifdef Q_OS_LINUX
    configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + c_dirDelimiter;
#endif
#endif
    return configDir;
}

QString Settings::dataDir()
{
    QString dataDir;
    dataDir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + c_dirDelimiter;
    return dataDir;
}

QString Settings::workDir()
{
    return s_workDir;
}

void Settings::setWorkDir(const QString &dir)
{
    s_workDir = dir;
    if (!s_workDir.endsWith(c_dirDelimiter))
        s_workDir += c_dirDelimiter;
    set("workDir", s_workDir);
}

QString Settings::logDir()
{
    return dataDir() + "logs/";
}

void Settings::initialize()
{
    for (const QString &dirstr : { configDir(), dataDir(), logDir() })
    {
        QDir dir(dirstr);
        if (!dir.exists())
            dir.mkdir(dirstr);
    }
    s_workDir = QString(get("workDir", dataDir()));
}

Settings &Settings::instance()
{
    static Settings singleton;
    return singleton;
}
