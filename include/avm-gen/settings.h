#pragma once

#include <QMap>
#include <QSettings>
#include <QString>
#include <avm-gen/utils/convertable.h>

class GENLIB_EXPORT Settings
{
public:
    /// \brief Private constructor — use initialize() and instance() instead.
    explicit Settings();

    /// \brief Pushes \a newGroup onto the QSettings group stack (calls beginGroup).
    static void pushGroup(const QString &newGroup);

    /// \brief Pops one level from the QSettings group stack (calls endGroup).
    static void popGroup();

    /*! \brief Returns child group names, optionally scoped to \a key.
     *  \param key  If non-empty, temporarily enters this group before listing.
     *  \return List of child group names.
     */
    static QStringList groups(const QString &key = "");

    /*! \brief Returns the stored value for \a key, or \a defValue if not present.
     *  \details Returns \a defValue immediately (with a warning) if initialize() has not
     *           been called yet.
     */
    static utils::Convertable get(const QString &key, const QVariant &defValue = QVariant());

    /// \brief Stores \a value under \a key and immediately syncs to disk.
    static void set(const QString &key, const QVariant &value);

    /// \brief Removes the entry identified by \a name from persistent storage.
    static void remove(const QString &name);

    /*! \brief Returns true if \a name exists as a key in the current settings scope.
     *  \note This checks for keys, not groups; use groups() to inspect child groups.
     */
    static bool groupExist(const QString &name);

    /// \brief Returns the platform-specific writable configuration directory (with trailing separator).
    /// under Windows it is C:/Users/<USER>/AppData/Roaming/<ORGNAME>/<APPNAME>
    /// under Linux it is ~/.config/<ORGNAME>/<APPNAME>
    /// \returns QString directory string
    static QString configDir();

    /// \brief Returns the platform-specific writable application data directory (with trailing separator).
    /// under Windows it is C:/Users/<USER>/AppData/Local/<ORGNAME>/<APPNAME>
    /// under Linux it is ~/.local/share/<ORGNAME>/<APPNAME>
    /// \returns QString directory string
    static QString dataDir();

    /// \brief Returns the currently configured working directory (with trailing separator).
    /// under Windows it is C:/Users/<USER>/AppData/Local/<ORGNAME>/<APPNAME>
    /// under Linux it is ~/.local/share/<ORGNAME>/<APPNAME>
    /// \returns QString directory string
    static QString workDir();

    /*! \brief Stores \a dir as the working directory, appending a trailing separator if needed.
     *  \details Persists the value via set("workDir", ...) so it survives restarts.
     *  \param QString dir - path to change workdir to
     */
    static void setWorkDir(const QString &dir);

    /// \brief Returns the log directory path: dataDir() + "logs/".
    static QString logDir();

    /*! \brief Initialises persistent settings and creates required application directories.
     *  \param appName   Application name (used by QSettings and QStandardPaths).
     *  \param orgName   Organisation name.
     *  \param version   Application version string.
     *  \details Must be called once before any other Settings method.
     *           Creates configDir(), dataDir(), and logDir() if they do not exist.
     */
    static void initialize(const QString &appName, const QString &orgName, const QString &version);

private:
    /*! \brief Returns the single Settings instance.
     *  \pre initialize() must have been called; asserts otherwise in debug builds.
     */
    static Settings &instance();
    static QString s_workDir;
    static bool s_settingsPathUpdated;
    QSettings conf;
};
