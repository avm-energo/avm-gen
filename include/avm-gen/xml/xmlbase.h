#pragma once

#include <QDomElement>
#include <QObject>
#include <avm-gen/gen_export.h>

class XmlBase
{
public:
    XmlBase();

    /// \brief Loads DOM Document from ByteArray and returns QDomElement
    static bool GENLIB_EXPORT getXMLFromByteArray(const QByteArray &ba, QDomElement &element);

    /// \brief Returns ByteArray from DOM Document
    static QByteArray GENLIB_EXPORT toByteArray();

    /// \brief Returns QDomDocument for a file with the specified filename from user's local folder.
    static QDomElement GENLIB_EXPORT getXMLFirstElementFromFile(const QString &filename, const QString &tag);

private:
    static QDomDocument s_xmlDocument;
};
