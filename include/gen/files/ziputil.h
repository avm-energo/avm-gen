#include <QFile>
#include <gen/error.h>
#include <gen/gen_export.h>
#include <zip.h>

class GENLIB_EXPORT ZipUtil
{
private:
    ZipUtil() { };

public:
    ZipUtil(ZipUtil const &) = delete;
    void operator=(ZipUtil const &) = delete;

    static ZipUtil &getInstance();
    /// \brief Create archive \a zipFileName with only one file \a filename in CREATE mode
    static Error::Msg CompressFile(const QString &filename, const QString &zipFileName);
    /// \brief Create archive \a zipFileName with directory \a dirname in CREATE mode
    static Error::Msg CompressDir(const QString &dirname, const QString &zipFileName);
    /// \brief Adds one file to archive in ADD mode
    /// \param zipFileName - archive name
    /// \param filename - the name of file in archive and the name of file to add if \a ba is empty
    /// \param ba [optional] - source for file
    static Error::Msg AddFile(const QString &filename, const QString &zipFileName, const QByteArray &ba = QByteArray());
    /// \brief Uncompress archive \a zipFileName contents to the directory \a dirname
    static Error::Msg DecompressFile(const QString &zipFileName, const QString &dirname);
    /// \brief Uncompress file from zip archive
    /// \param zipFileName - archive name
    /// \param filename - filename from archive
    /// \return QByteArray - the contents of filename if found, empty QByteArray if not
    static QByteArray DecompressFileToBA(const QString &zipFileName, const QString &filename);
    /// \brief Get archive \a zipFileName contents in QStringList with full paths
    static QStringList GetArchiveContentsList(const QString &zipFileName);

private:
    static bool addFile(zip_t *za, const QString &filenameToAdd, const QString &zipFileName);
    static bool addFileBA(zip_t *za, const QByteArray &ba, const QString &zipFileName);
    static bool addDir(zip_t *za, const QString &dirToAdd, const QString &parentDir = "");
};
