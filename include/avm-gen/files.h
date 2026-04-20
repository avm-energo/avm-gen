#pragma once

#include <QDateTime>
#include <QFile>
#include <avm-gen/error.h>
#include <avm-gen/gen_export.h>

namespace Files
{
constexpr int FILENAMEMAXSYMBOLS = 10;

/*! \brief Returns \a filenamestr when non-empty, otherwise returns \a res.
 *  \details Convenience helper: if the user supplied an explicit save name, use it;
 *           otherwise fall back to the auto-generated name \a res.
 */
QString GENLIB_EXPORT ChooseFileForSave(const QString &res, const QString &filenamestr = QLatin1String(""));

/*! \brief Returns true if \a filename exists inside Settings::configDir().
 *  \note Only searches the config directory — pass an absolute path to check elsewhere.
 */
bool GENLIB_EXPORT isFileExist(const QString &filename);

Error::Msg GENLIB_EXPORT LoadFromFile(const QString &filename, QByteArray &ba);
Error::Msg GENLIB_EXPORT SaveToFile(const QString &filename, const QByteArray &src);

/// \brief Returns a list of root paths for all currently mounted drives/volumes.
QStringList GENLIB_EXPORT Drives();

/*! \brief Searches each directory in \a filepaths for files matching \a filename.
 *  \param filepaths  Directories to search (const — the list is never modified).
 *  \param filename   Filename pattern (supports wildcards via QDirIterator).
 *  \param subdirs    If true, recurses into sub-directories.
 *  \return List of absolute paths of all matching files found.
 */
QStringList GENLIB_EXPORT SearchForFile(const QStringList &di, const QString &filename, bool subdirs = false);

/*! \brief Returns the first path in \a filepaths whose storage volume has the given \a label.
 *  \param filepaths  Candidate root paths to check (const — not modified).
 *  \param label      Volume label to match.
 *  \return Matching path, or an empty string if no match is found.
 */
QString GENLIB_EXPORT GetFirstDriveWithLabel(const QStringList &filepaths, const QString &label);

/*! \brief Compresses and rotates \a logFile when it exceeds LOG_MAX_SIZE bytes.
 *  \details Rotates existing compressed archives (e.g., .0.zip → .1.zip, …) and then
 *           compresses the current log into .0.zip, truncating the live log to zero.
 *           Called after every write so log files never grow beyond ~1 MB on disk.
 */
void GENLIB_EXPORT checkNGzip(QFile *logFile);

/*! \brief Rotates numbered archive files: shifts .N.ext → .(N+1).ext for N = count-1..0.
 *  \param path       Base file path (without numeric index or extension).
 *  \param extension  Extension of the archive files (default "zip").
 *  \param count      Maximum number of rotated files to keep (default 9).
 *  \return true on success, false if any rename operation fails.
 */
bool GENLIB_EXPORT rotateFiles(const QString &path, const QString &extension = QLatin1String("zip"), int count = 9);

/*! \brief Renames \a oldFileName to \a newFileName.
 *  \return true on success, false if the source does not exist or the rename fails.
 */
bool GENLIB_EXPORT rename(const QString &oldFileName, const QString &newFileName);

/// \brief Creates all directories in the path leading to \a path (including any missing parents).
void GENLIB_EXPORT makePath(const QFile &path);

/*! \brief Writes \a src to a new temporary file and returns its path.
 *  \details The file is created with AutoRemove = false, so the caller is responsible
 *           for deleting it when finished.
 *  \return Absolute path of the temporary file, or an empty string on failure.
 */
QString GENLIB_EXPORT SaveToTempFile(const QByteArray &src);

/*! \brief Removes files in \a dir matching \a filenameMask that are older than \a datetime.
 *  \param dir           Directory to scan.
 *  \param filenameMask  Wildcard pattern (e.g. "backup*.zip").
 *  \param datetime      Files with lastModified() before this are removed.
 */
void GENLIB_EXPORT removeOlderThan(
    const QString &dir, const QString &filenameMask, const QDateTime &datetime = QDateTime::currentDateTime());

/// \brief Extracts and returns the filename component from \a filePath (no directory).
QString GENLIB_EXPORT getFileName(const QString &filePath);

/// \brief Returns the directory component of \a filePath (without trailing separator).
QString GENLIB_EXPORT getDirName(const QString &filePath);
} // namespace Files
