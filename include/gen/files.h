#pragma once

#include <QDateTime>
#include <QFile>
#include <gen/error.h>
#include <gen/gen_export.h>

namespace Files
{
constexpr int FILENAMEMAXSYMBOLS = 10;
QString GENLIB_EXPORT ChooseFileForSave(const QString &res, const QString &filenamestr = QLatin1String(""));
bool GENLIB_EXPORT isFileExist(const QString &filename);
Error::Msg GENLIB_EXPORT LoadFromFile(const QString &filename, QByteArray &ba);
Error::Msg GENLIB_EXPORT SaveToFile(const QString &filename, const QByteArray &src);
QStringList GENLIB_EXPORT Drives();
QStringList GENLIB_EXPORT SearchForFile(QStringList &di, const QString &filename, bool subdirs = false);
QString GENLIB_EXPORT GetFirstDriveWithLabel(QStringList &filepaths, const QString &label);
void GENLIB_EXPORT checkNGzip(QFile *logFile);
bool GENLIB_EXPORT rotateFiles(const QString &path, const QString &extension = QLatin1String("zip"), int count = 9);
bool GENLIB_EXPORT rename(const QString &oldFileName, const QString &newFileName);
void GENLIB_EXPORT makePath(const QFile &path);

/// \brief Saves bytearray to temporary file
/// \param src - buffer to write
/// \return name of the file
const QString GENLIB_EXPORT SaveToTempFile(const QByteArray &src);

/// \brief Removes files older than datetime
/// \param dir - directory to analyze
/// \param filenameMask - files mask, ex. "backup*.zip"
/// \param datetime - datetime in the past before that files will be removed

void GENLIB_EXPORT removeOlderThan(
    const QString &dir, const QString &filenameMask, const QDateTime &datetime = QDateTime::currentDateTime());

/// \brief Returns filename without path to it
/// \param filepath - full path with file name
/// \return filename without path
QString GENLIB_EXPORT getFileName(const QString &filePath);

/// \brief Returns directory of the file
/// \param filepath - full path with file name
/// \return Directory name
QString GENLIB_EXPORT getDirName(const QString &filePath);
} // namespace Files
