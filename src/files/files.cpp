#include <QDirIterator>
#include <QIODevice>
#include <QRandomGenerator>
#include <QStorageInfo>
#include <QTemporaryFile>
#include <QtDebug>
#include <gen/files.h>
#include <gen/files/ziputil.h>
#include <gen/settings.h>
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
    auto dir = QDir(Settings::configDir());
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
        if (rotateFiles(filename))
        {
            auto &zip = ZipUtil::getInstance();
            if (zip.CompressFile(filename, filename + ".0.zip") == Error::Msg::NoError)
            {
                logFile->resize(0);
                logFile->flush();
            }
        }
    }
}

bool Files::rotateFiles(const QString &path, const QString &extension, int count)
{
    const QString ext = "." + extension;
    // rotating
    for (int i = count; i > 0; --i)
    {
        auto tempNew = path + "." + QString::number(i) + ext;
        auto tempOld = path + "." + QString::number(i - 1) + ext;
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

const QString Files::SaveToTempFile(const QByteArray &src)
{
    QTemporaryFile file;
    file.setAutoRemove(false);
    file.open();
    file.write(src);
    file.close();
    return file.fileName();
}

void Files::removeOlderThan(const QString &dir, const QString &filenameMask, const QDateTime &datetime)
{
    QDir dirToCheck(dir);
    dirToCheck.setNameFilters(QStringList(filenameMask));
    dirToCheck.setSorting(QDir::Time | QDir::Reversed);
    QFileInfoList flist = dirToCheck.entryInfoList();
    for (QFileInfo fi : flist)
    {
        if (fi.lastModified().secsTo(datetime) > 0) // time of file is lower
            QFile::remove(fi.filePath());
    }
}

QString Files::getFileName(const QString &filePath)
{
    QFileInfo fi(filePath);
    return fi.fileName();
}

QString Files::getDirName(const QString &filePath)
{
    QFileInfo fi(filePath);
    return fi.path();
}
