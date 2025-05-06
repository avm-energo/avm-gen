#pragma once

#include <QMap>
#include <QSettings>
#include <QString>

class Settings
{
public:
    explicit Settings();

    static QVariant value(const QString &key, const QVariant &defValue = QVariant());
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

    /// \brief Get work dir, user modifyable
    /// \details Get work dir:
    /// under Windows it is C:/Users/<USER>/AppData/Local/<ORGNAME>/<APPNAME>
    /// under Linux it is ~/.local/share/<ORGNAME>/<APPNAME>
    /// \returns QString directory string
    static QString workDir();

    /// \brief Modify work dir
    /// \details Set work dir:
    /// \param QString dir - path to change workdir to
    static void setWorkDir(const QString &dir);

    static QString logDir();

private:
    static QString m_workDir;
};
