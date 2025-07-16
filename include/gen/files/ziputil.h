#include <QFile>
#include <gen/error.h>
#include <zip.h>

class ZipUtil
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
};
