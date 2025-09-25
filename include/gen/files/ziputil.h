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
    static Error::Msg DecompressFile(const QString &zipFileName, const QString &dirname);

private:
    static bool addFile(zip_t *za, const QString &filenameToAdd, const QString &zipFileName);
    static bool addDir(zip_t *za, const QString &dirToAdd, const QString &parentDir = "");
};
