#pragma once

#include <QDomElement>
#include <QObject>
#include <gen/gen_export.h>

class XmlBase : public QObject
{
    Q_OBJECT
public:
    XmlBase();

    /// \brief Loads DOM Document from ByteArray and returns QDomElement
    static bool GENLIB_EXPORT getXMLFromByteArray(const QByteArray &ba, QDomElement &element);

    /// \brief Returns ByteArray from DOM Document
    static QByteArray GENLIB_EXPORT toByteArray();

    /// \brief Returns QDomDocument for a file with the specified filename from user's local folder.
    QDomElement GENLIB_EXPORT getXMLFirstElementFromFile(const QString &filename, const QString &tag);

signals:
    void error();

private:
    static QDomDocument s_xmlDocument;
};
