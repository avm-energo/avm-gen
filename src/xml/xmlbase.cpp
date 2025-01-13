#include <QDir>
#include <QFile>
#include <QtDebug>
#include <gen/stdfunc.h>
#include <gen/xml/xmlbase.h>

QDomDocument XmlBase::s_xmlDocument = QDomDocument();

XmlBase::XmlBase() : QObject()
{
}

bool XmlBase::getXMLFromByteArray(const QByteArray &ba, QDomElement &element)
{
    QDomDocument::ParseResult result = s_xmlDocument.setContent(ba);
    if (!result.errorMessage.isEmpty())
    {
        qDebug() << "Error while parsing bytearray, line: " << result.errorLine << ", column: " << result.errorColumn
                 << "\nError message is: " << result.errorMessage;
        return false;
    }
    // recurivelly change color
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
    QDir dir(StdFunc::GetSystemHomeDir());
    QFile file(dir.filePath(filename));
    if (file.exists())
    {
        if (file.open(QIODevice::ReadOnly))
        {
            QDomDocument::ParseResult res = doc.setContent(&file);
            file.close();
            if (res)
            {
                if (!doc.isNull())
                {
                    element = doc.firstChildElement(tag);
                    return element;
                }
                else
                    qDebug() << "File is empty: " << filename;
            }
            else
            {
                qDebug() << "File: " << filename << " Error: " + res.errorMessage << //
                    " Line: " << res.errorLine << " Column: " << res.errorColumn;
            }
        }
        else
            qDebug() << "File open error: " << filename;
    }
    else
        qDebug() << "File not found: " << filename;
    emit error();
    return QDomElement();
}
