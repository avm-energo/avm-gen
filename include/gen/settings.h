#pragma once

#include <QMap>
#include <QSettings>
#include <QString>

class Settings
{
public:
    explicit Settings();

    static QVariant value(const QString &key, const QVariant &defValue);
    static void setValue(const QString &key, const QVariant &value);

    /// \brief Get config dir
    /// \details Get config dir:
    /// under Windows it is C:/Users/<USER>/AppData/Roaming/<ORGNAME>/<APPNAME>
    /// under Linux it is ~/.config/<ORGNAME>/<APPNAME>
    /// \returns QString directory string
    static QString configDir();

    /// \brief Get data dir
    /// \details Get data dir:
    /// under Windows it is C:/Users/<USER>/AppData/Local/<ORGNAME>/<APPNAME>
    /// under Linux it is ~/.local/share/<ORGNAME>/<APPNAME>
    /// \returns QString directory string
    static QString dataDir();

    static QString workDir();
    static void setWorkDir(const QString &dir);

private:
    static QString m_workDir;
};
