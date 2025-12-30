#include <QDir>
#include <QFile>
#include <QtDebug>
#include <avm-gen/settings.h>
#include <avm-gen/xml/xmlbase.h>

QDomDocument XmlBase::s_xmlDocument = QDomDocument();

XmlBase::XmlBase()
{
}

bool XmlBase::getXMLFromByteArray(const QByteArray &ba, QDomElement &element)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
    QString errstr;
    int errline, errcolumn;
    if (!(s_xmlDocument.setContent(ba, &errstr, &errline, &errcolumn)))
    {
        qDebug() << "Error while parsing bytearray, line: " << errline << ", column: " << errcolumn
                 << "\nError message is: " << errstr;
        return false;
    }
#else
    QDomDocument::ParseResult result = s_xmlDocument.setContent(ba);
    if (!result.errorMessage.isEmpty())
    {
        qDebug() << "Error while parsing bytearray, line: " << result.errorLine << ", column: " << result.errorColumn
                 << "\nError message is: " << result.errorMessage;
        return false;
    }
    // recurivelly change color
#endif
    element = s_xmlDocument.documentElement();
    return true;
}

QByteArray XmlBase::toByteArray()
{
    if (!s_xmlDocument.isNull())
        return s_xmlDocument.toByteArray();
    else
        return QByteArray();
}

QDomElement XmlBase::getXMLFirstElementFromFile(const QString &filename, const QString &tag)
{
    QDomDocument doc;
    QDomElement element;
    QDir dir((Settings::configDir()));
    QFile file(dir.filePath(filename));
    if (file.exists())
    {
        if (file.open(QIODevice::ReadOnly))
        {
#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
            QString errstr;
            int errline, errcolumn;
            if (doc.setContent(&file, &errstr, &errline, &errcolumn))
            {
#else
            QDomDocument::ParseResult res = doc.setContent(&file);
            if (res)
            {
#endif
                file.close();
                if (!doc.isNull())
                {
                    element = doc.firstChildElement(tag);
                    return element;
                }
                else
                    qDebug() << "File is empty: " << dir.filePath(filename);
            }
            else
            {
                file.close();
#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
                qDebug() << "Error while parsing bytearray, line: " << errline << ", column: " << errcolumn
                         << "\nError message is: " << errstr;
#else
                qDebug() << "File: " << dir.filePath(filename) << " Error: " + res.errorMessage << //
                    " Line: " << res.errorLine << " Column: " << res.errorColumn;
#endif
            }
        }
        else
            qDebug() << "File open error: " << dir.filePath(filename);
    }
    else
        qDebug() << "File not found: " << dir.filePath(filename);
    return QDomElement();
}
