#include <QDirIterator>
#include <QIODevice>
#include <QStorageInfo>
#include <QtDebug>
#include <gen/files.h>
#include <gen/files/lzma_util.h>
#include <gen/stdfunc.h>

constexpr auto LOG_MAX_SIZE = 1048576;

QString Files::ChooseFileForSave(const QString &res, const QString &filenamestr)
{
    if (!filenamestr.isEmpty())
        return filenamestr;
    else
        return res;
}

bool Files::isFileExist(const QString &filename)
{
    auto dir = QDir(StdFunc::GetSystemHomeDir());
    return QFile::exists(dir.filePath(filename));
}

Error::Msg Files::LoadFromFile(const QString &filename, QByteArray &ba)
{
    if (filename.isEmpty())
    {
        qCritical("Пустое имя файла");
        return Error::Msg::FileNameError; // Пустое имя файла
    }
    QFile file(filename); // QFile must be allocated on stack
    if (!file.open(QIODevice::ReadOnly))
    {
        qCritical("Ошибка открытия файла");
        return Error::Msg::FileOpenError; // Ошибка открытия файла
    }
    ba = file.readAll();
    file.close();
    return Error::Msg::NoError;
}

Error::Msg Files::SaveToFile(const QString &filename, const QByteArray &src)
{
    if (filename.isEmpty())
    {
        qCritical("Пустое имя файла");
        return Error::Msg::FileNameError; // Пустое имя файла
    }
    QFile file(filename); // QFile must be allocated on stack
    if (!file.open(QIODevice::WriteOnly))
        return Error::Msg::FileOpenError; // Ошибка открытия файла
    if (file.write(src, src.size()) != -1)
    {
        // нет ошибок
        file.close();
        return Error::Msg::NoError;
    }
    else
    {
        // ошибка записи
        file.close();
        return Error::Msg::FileWriteError;
    }
}

QStringList Files::Drives()
{
    QStringList sl;
    QFileInfoList list = QDir::drives();
    for (const QFileInfo &fi : list)
        sl << fi.path();
    return sl;
}

QStringList Files::SearchForFile(QStringList &filepaths, const QString &filename, bool subdirs)
{
    QStringList files;
    for (const QString &filepath : filepaths)
    {
        QStringList sl = QStringList() << filename;
        QDirIterator it(filepath, sl, QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot,
            (subdirs) ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);
        while (it.hasNext())
            files << it.next();
    }
    return files;
}

QString Files::GetFirstDriveWithLabel(QStringList &filepaths, const QString &label)
{
    QString str = "";
    int i = 0;
    while ((str.isEmpty()) && (i < filepaths.size()))
    {
        QStorageInfo si;
        si.setPath(filepaths.at(i));
        if (si.name() == label)
            str = filepaths.at(i);
        ++i;
    }
    return str;
}

void Files::checkNGzip(QFile *logFile)
{
    if (logFile->size() >= LOG_MAX_SIZE)
    {
        auto filename = logFile->fileName();
        if (rotateGzipLogs(filename))
        {
            QFile fileOut(filename + ".0.gz");
            if (fileOut.open(QIODevice::WriteOnly | QIODevice::Truncate))
            {
                auto &lzma = LzmaUtil::GetInstance();
                logFile->seek(0);
                auto bytes = logFile->readAll();
                auto compressed = lzma.compress(bytes);
                auto written = fileOut.write(compressed);
                if (written == -1)
                    qCritical("Writing gz file error");
                logFile->resize(0);
                logFile->flush();
                fileOut.flush();
                fileOut.close();
            }
            else
                qWarning() << "Cannot open the file" << fileOut.fileName();
        }
    }
}

bool Files::rotateGzipLogs(const QString &path)
{
    // rotating
    for (int i = 9; i > 0; --i)
    {
        auto tempNew = path + "." + QString::number(i) + ".gz";
        auto tempOld = path + "." + QString::number(i - 1) + ".gz";
        QFile fn(tempNew);
        if (fn.exists())
            fn.remove();
        fn.setFileName(tempOld);
        if (fn.exists())
        {
            if (fn.rename(tempNew) == false) // error
            {
                ERMSG("Cannot rename file");
                return false;
            }
        }
    }
    return true;
}

void Files::makePath(const QFile &path)
{
    QFileInfo fi(path);
    QDir().mkpath(fi.dir().path());
}
