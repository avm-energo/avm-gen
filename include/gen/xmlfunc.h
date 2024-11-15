#pragma once

#include <QByteArray>
#include <QDomDocument>
#include <QStringList>

class XmlFunc
{
public:
    XmlFunc();

    /// \brief Load bytearray contents to xml document and edit contents changing attributes attrs to newvalues

    static void replaceDomWithNewAttrRecursively(QByteArray &data, const QString &name, const QString &attrname,
        const QStringList &attrs, const QString &attrcheckvalue, const QStringList &newvalues);

    /// \brief Recursively parse xml contents and edit contents changing attributes attrs to newvalues

    static void replaceDomWithNewAttrRecursively(QDomElement &root, const QString &name, const QString &attrname,
        const QStringList &attrs, const QString &attrcheckvalue, const QStringList &newvalues);

private:
    static QString getAttrValue(const QString &string, const QString &attrname);
};
