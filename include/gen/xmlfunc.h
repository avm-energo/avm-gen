#pragma once

#include <QByteArray>
#include <QDomDocument>
#include <QStringList>

class XmlFunc
{
public:
    struct AttrsSearchedStruct
    {
        QString DomElementName; // the name of DOM element to find required attr in
        QString AttrName;       // the name of Attr in DOM element to find parameter name and value in
        QString ParameterName;  // the name of parameter to check value of
        QString ParameterValue; // the value of the parameter to check
    };

    XmlFunc();

    /// \brief Load bytearray contents to xml document and edit contents changing attributes attrs to newvalues

    static void replaceComplicatedAttrRecursively(
        QByteArray &data, AttrsSearchedStruct &findInfo, const QStringList &attrs, const QStringList &newvalues);

    static void replaceSimpleAttr(
        QByteArray &data, const QString &elementName, QStringList &attrs, QStringList &newvalues);

    /// \brief Recursively parse xml contents and edit contents changing attributes attrs to newvalues
    /// check ParameterName in AttrName to contain ParameterValue and if so replace all attrs oldvalues with newvalues

    static void replaceDomWithNewAttrRecursively(
        QDomElement &root, AttrsSearchedStruct &findInfo, const QStringList &attrs, const QStringList &newvalues);

private:
    static QString getAttrValue(QString &string, const QString &attrname);
};
