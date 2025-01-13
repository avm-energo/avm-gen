#pragma once

#include <QByteArray>
#include <QDomDocument>
#include <QStringList>
#include <gen/gen_export.h>

class XmlAttr
{
public:
    struct AttrsSearchedStruct
    {
        QString DomElementName; // the name of DOM element to find required attr in
        QString AttrName;       // the name of Attr in DOM element to find parameter name and value in
        QString ParameterName;  // the name of parameter to check value of
        QString ParameterValue; // the value of the parameter to check
    };

    XmlAttr();

    /// \brief Load bytearray contents to xml document and edit contents changing attributes attrs to newvalues

    static bool GENLIB_EXPORT replaceComplicatedAttrRecursively(
        QByteArray &data, AttrsSearchedStruct &findInfo, const QStringList &attrs, const QStringList &newvalues);

    static bool GENLIB_EXPORT replaceSimpleAttr(
        QByteArray &data, const QString &elementName, QStringList &attrs, QStringList &newvalues);

    /// \brief Recursively parse xml contents and edit contents changing attributes attrs to newvalues
    /// check ParameterName in AttrName to contain ParameterValue and if so replace all attrs oldvalues with newvalues

    static void GENLIB_EXPORT replaceDomWithNewAttrRecursively(
        QDomElement &root, AttrsSearchedStruct &findInfo, const QStringList &attrs, const QStringList &newvalues);

private:
    static QString getAttrValue(QString &string, const QString &attrname);
};
