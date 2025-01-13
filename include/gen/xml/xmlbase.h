#pragma once

#include <QDomElement>
#include <QObject>

class XmlBase : public QObject
{
    Q_OBJECT
public:
    XmlBase();

    /// \brief Loads DOM Document from ByteArray and returns QDomElement
    static bool getXMLFromByteArray(const QByteArray &ba, QDomElement &element);

    /// \brief Returns ByteArray from DOM Document
    static QByteArray toByteArray();

    /// \brief Returns QDomDocument for a file with the specified filename from user's local folder.
    QDomElement getXMLFirstElementFromFile(const QString &filename, const QString &tag);

signals:
    void error();

private:
    static QDomDocument s_xmlDocument;
};
