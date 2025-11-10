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
    std::string zipfilenamestr = zipFileName.toStdString();

    if ((za = zip_open(zipfilenamestr.c_str(), ZIP_CREATE, &err)) == nullptr)
    {
        zip_error_t error;
        zip_error_init_with_code(&error, err);
        qDebug() << "Cannot open zip archive " << zipFileName << ", error is: " << zip_error_strerror(&error);
        zip_error_fini(&error);
        return Error::Msg::FileOpenError;
    }

    QFileInfo fi(filename);
    if (!addFile(za, filename, fi.fileName()))
        return Error::Msg::FileWriteError;

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

    QDir dir(dirname);
    QStringList files = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
    for (auto file : files)
        addFile(za, QString("%1/%2").arg(dir.absolutePath(), file), file);

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
            zip_int64_t len = strlen(sb.name);
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
            qFatal("Zip_stat_index error");
    }

    if ((zip_close(za)) < 0)
    {
        qDebug() << "Cannot close archive " << zipFileName << ", error: " << zip_strerror(za);
        zip_discard(za);
        return Error::Msg::FileWriteError;
    }
    return Error::Msg::NoError;
}

QByteArray ZipUtil::DecompressFileToBA(const QString &zipFileName, const QString &filename)
{
    zip_t *za;
    struct zip_file *zf;
    struct zip_stat sb;
    int err;
    std::string filenamestr = filename.toStdString();
    std::string zipfilenamestr = zipFileName.toStdString();
    QByteArray ba;

    if ((za = zip_open(zipfilenamestr.c_str(), 0, &err)) == nullptr)
    {
        zip_error_t error;
        zip_error_init_with_code(&error, err);
        qDebug() << "Cannot open zip archive " << zipFileName << ", error is: " << zip_error_strerror(&error);
        zip_error_fini(&error);
        return ba;
    }

    for (int i = 0; i < zip_get_num_entries(za, 0); i++)
    {
        if (zip_stat_index(za, i, 0, &sb) == 0)
        {
            if (strcmp(sb.name, filenamestr.c_str()) != 0)
                continue;
            zf = zip_fopen_index(za, i, 0);
            if (!zf)
            {
                qDebug() << "Cannot open file in zip: " << sb.name;
                return ba;
            }

            zip_uint64_t sum = 0;
            QByteArray tmpba;
            while (sum != sb.size)
            {
                zip_int64_t len = zip_fread(zf, tmpba.data(), 100);
                if (len < 0)
                {
                    qDebug() << "Cannot read file in zip: " << sb.name;
                    return ba;
                }
                ba += tmpba;
                sum += len;
            }
            zip_fclose(zf);
        }
        else
            qFatal("Zip_stat_index error");
    }

    if ((zip_close(za)) < 0)
    {
        qDebug() << "Cannot close archive " << zipFileName << ", error: " << zip_strerror(za);
        zip_discard(za);
        return ba;
    }
    return ba;
}

QStringList ZipUtil::getArchiveContentsList(const QString &zipFileName)
{
    zip_t *za;
    struct zip_stat sb;
    int err;
    std::string zipfilenamestr = zipFileName.toStdString();
    QStringList sl;

    if ((za = zip_open(zipfilenamestr.c_str(), 0, &err)) == nullptr)
    {
        zip_error_t error;
        zip_error_init_with_code(&error, err);
        qDebug() << "Cannot open zip archive " << zipFileName << ", error is: " << zip_error_strerror(&error);
        zip_error_fini(&error);
        return sl;
    }

    for (int i = 0; i < zip_get_num_entries(za, 0); i++)
    {
        if (zip_stat_index(za, i, 0, &sb) == 0)
        {
            sl << sb.name;
        }
        else
            qFatal("Zip_stat_index error");
    }
    if ((zip_close(za)) < 0)
    {
        qDebug() << "Cannot close archive " << zipFileName << ", error: " << zip_strerror(za);
        zip_discard(za);
        return sl;
    }
    return sl;
}

bool ZipUtil::addFile(zip_t *za, const QString &filenameToAdd, const QString &zipFileName)
{
    zip_source_t *sourceFile;

    std::string fileNameStr = filenameToAdd.toStdString();
    std::string zipFileNameStr = zipFileName.toStdString();

    /* The data can come from any source. To keep the example simple, it is provided in a static buffer here. */
    if ((sourceFile = zip_source_file(za, fileNameStr.c_str(), 0, 0)) == nullptr)
    {
        qDebug() << "Cannot create buffer source: " << zip_strerror(za);
        zip_discard(za);
        return false;
    }

    if ((zip_file_add(za, zipFileNameStr.c_str(), sourceFile, ZIP_FL_OVERWRITE)) < 0)
    {
        qDebug() << "Cannot add precompressed file: " << zip_strerror(za);
        zip_source_free(sourceFile);
        zip_discard(za);
        return false;
    }
    return true;
}
