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
    static Error::Msg CompressFile(const QString &filename, const QString &zipFileName);
    static Error::Msg CompressDir(const QString &dirname, const QString &zipFileName);
    /// \brief Uncompress archive zipFileName contents to the directory dirname
    static Error::Msg DecompressFile(const QString &zipFileName, const QString &dirname);
    /// \brief Uncompress file from zip archive
    /// \param zipFileName - archive name
    /// \param filename - filename from archive
    /// \return QByteArray - the contents of filename if found, empty QByteArray if not
    static QByteArray DecompressFileToBA(const QString &zipFileName, const QString &filename);
    /// \brief Get archive zipFileName contents in QStringList with full paths
    static QStringList getArchiveContentsList(const QString &zipFileName);

private:
    static bool addFile(zip_t *za, const QString &filenameToAdd, const QString &zipFileName);
    static bool addDir(zip_t *za, const QString &dirToAdd, const QString &parentDir = "");
};
