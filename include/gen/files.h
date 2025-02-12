#pragma once

#include <QFile>
#include <gen/error.h>
#include <gen/gen_export.h>

namespace Files
{
constexpr int FILENAMEMAXSYMBOLS = 10;
QString GENLIB_EXPORT ChooseFileForSave(const QString &res, const QString &filenamestr = "");
bool GENLIB_EXPORT isFileExist(const QString &filename);
Error::Msg GENLIB_EXPORT LoadFromFile(const QString &filename, QByteArray &ba);
Error::Msg GENLIB_EXPORT SaveToFile(const QString &filename, const QByteArray &src);
QStringList GENLIB_EXPORT Drives();
QStringList GENLIB_EXPORT SearchForFile(QStringList &di, const QString &filename, bool subdirs = false);
QString GENLIB_EXPORT GetFirstDriveWithLabel(QStringList &filepaths, const QString &label);
void GENLIB_EXPORT checkNGzip(QFile *logFile);
bool GENLIB_EXPORT rotateGzipLogs(const QString &path);
void GENLIB_EXPORT makePath(const QFile &path);

/// \brief Saves bytearray to temporary file
/// \param src - buffer to write
/// \return name of the file
const QString GENLIB_EXPORT SaveToTempFile(const QByteArray &src);
} // namespace Files
