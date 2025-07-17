#include <QDir>
#include <QFileInfo>
#include <QtDebug>
#include <gen/files/ziputil.h>
#include <string>

ZipUtil &ZipUtil::getInstance()
{
    static ZipUtil instance;
    return instance;
}

Error::Msg ZipUtil::CompressFile(const QString &filename, const QString &zipFileName)
{
    zip_t *za;
    int err;
    zip_source_t *sourceFile;
    std::string filenamestr = filename.toStdString();
    std::string zipfilenamestr = zipFileName.toStdString();

    QFileInfo fi(QString::fromStdString(filenamestr));
    QString fileDirName = fi.dir().path();
    QString fileFileName = fi.fileName();
    std::string charfileFileName = fileFileName.toStdString();

    if ((za = zip_open(zipfilenamestr.c_str(), ZIP_CREATE, &err)) == nullptr)
    {
        zip_error_t error;
        zip_error_init_with_code(&error, err);
        qDebug() << "Cannot open zip archive " << zipFileName << ", error is: " << zip_error_strerror(&error);
        zip_error_fini(&error);
        return Error::Msg::FileOpenError;
    }

    /* The data can come from any source. To keep the example simple, it is provided in a static buffer here. */
    if ((sourceFile = zip_source_file(za, filenamestr.c_str(), 0, 0)) == nullptr)
    {
        qDebug() << "Cannot create buffer source: " << zip_strerror(za);
        zip_discard(za);
        return Error::Msg::FileWriteError;
    }

    if ((zip_file_add(za, charfileFileName.c_str(), sourceFile, ZIP_FL_OVERWRITE)) < 0)
    {
        qDebug() << "Cannot add precompressed file: " << zip_strerror(za);
        zip_source_free(sourceFile);
        zip_discard(za);
        return Error::Msg::FileWriteError;
    }

    if ((zip_close(za)) < 0)
    {
        qDebug() << "Cannot close archive " << zipFileName << ", error: " << zip_strerror(za);
        zip_discard(za);
        return Error::Msg::FileWriteError;
    }
    return Error::Msg::NoError;
}

Error::Msg ZipUtil::CompressDir(const QString &dirname, const QString &zipFileName)
{
    zip_t *za;
    int err;
    std::string dirnamestr = dirname.toStdString();
    std::string zipfilenamestr = zipFileName.toStdString();

    if ((za = zip_open(zipfilenamestr.c_str(), ZIP_CREATE, &err)) == nullptr)
    {
        zip_error_t error;
        zip_error_init_with_code(&error, err);
        qDebug() << "Cannot open zip archive " << zipFileName << ", error is: " << zip_error_strerror(&error);
        zip_error_fini(&error);
        return Error::Msg::FileOpenError;
    }

    if (zip_dir_add(za, dirnamestr.c_str(), ZIP_FL_ENC_GUESS) == -1)
    {
        qDebug() << "Cannot add dir " << dirname << " error: " << zip_strerror(za);
        zip_discard(za);
        return Error::Msg::FileWriteError;
    }

    if ((zip_close(za)) < 0)
    {
        qDebug() << "Cannot close archive " << zipFileName << ", error: " << zip_strerror(za);
        zip_discard(za);
        return Error::Msg::FileWriteError;
    }
    return Error::Msg::NoError;
}

Error::Msg ZipUtil::DecompressFile(const QString &zipFileName, const QString &dirname)
{
    zip_t *za;
    struct zip_file *zf;
    struct zip_stat sb;
    int err;
    std::string dirnamestr = dirname.toStdString();
    std::string zipfilenamestr = zipFileName.toStdString();

    if ((za = zip_open(zipfilenamestr.c_str(), 0, &err)) == nullptr)
    {
        zip_error_t error;
        zip_error_init_with_code(&error, err);
        qDebug() << "Cannot open zip archive " << zipFileName << ", error is: " << zip_error_strerror(&error);
        zip_error_fini(&error);
        return Error::Msg::FileOpenError;
    }

    for (int i = 0; i < zip_get_num_entries(za, 0); i++)
    {
        if (zip_stat_index(za, i, 0, &sb) == 0)
        {
            size_t len = strlen(sb.name);
            if (sb.name[len - 1] == '/')
            {
                QDir dir(sb.name);
                if (!dir.exists())
                    dir.mkpath(".");
            }
            else
            {
                zf = zip_fopen_index(za, i, 0);
                if (!zf)
                {
                    qDebug() << "Cannot open file in zip: " << sb.name;
                    return Error::Msg::FileOpenError;
                }

                QFile file(sb.name);
                if (!file.open(QIODevice::WriteOnly))
                {
                    qDebug() << "Cannot open file: " << sb.name;
                    return Error::Msg::FileOpenError;
                }

                zip_uint64_t sum = 0;
                QByteArray ba;
                while (sum != sb.size)
                {
                    len = zip_fread(zf, ba.data(), 100);
                    if (len < 0)
                    {
                        qDebug() << "Cannot read file in zip: " << sb.name;
                        return Error::Msg::ReadError;
                    }
                    file.write(ba);
                    sum += len;
                }
                file.close();
                zip_fclose(zf);
            }
        }
        else
            qFatal() << "Zip_stat_index error";
    }

    if ((zip_close(za)) < 0)
    {
        qDebug() << "Cannot close archive " << zipFileName << ", error: " << zip_strerror(za);
        zip_discard(za);
        return Error::Msg::FileWriteError;
    }
    return Error::Msg::NoError;
}
