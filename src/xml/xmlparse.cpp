#include <gen/xml/xmlparse.h>

XmlParse::XmlParse()
{
}

const QStringList XmlParse::parseArray(const QDomNode &node, const QString &tag)
{
    QStringList retList = {};
    auto strArrNode = node.firstChildElement(tag);
    if (!strArrNode.isNull() && strArrNode.hasChildNodes())
    {
        const auto &nodes = strArrNode.childNodes();
        if (!nodes.isEmpty())
        {
            const auto size = nodes.count();
            for (auto i = 0; i < size; i++)
            {
                auto strElem = nodes.item(i).toElement();
                if (!strElem.isNull())
                {
                    auto strItem = strElem.text();
                    retList.push_back(strItem);
                }
            }
        }
    }
    return retList;
}

const QString XmlParse::parseString(const QDomNode &node, const QString &tagName) const
{
    auto textNode = node.firstChildElement(tagName);
    if (!textNode.isNull())
        return textNode.text();
    else
        return "";
}

void XmlParse::callIfNodeExist(
    const QDomNode &parent, const QString &tagName, const std::function<void(const QDomNode &)> &functor)
{
    auto node = parent.firstChildElement(tagName);
    if (!node.isNull())
        functor(node);
}

void XmlParse::callForEachChild(const QDomNode &parent, const std::function<void(const QDomNode &)> &functor)
{
    auto child = parent.firstChild();
    while (!child.isNull())
    {
        if (!child.isComment() && child.isElement())
            functor(child);
        child = child.nextSibling();
    }
}

void XmlParse::parseNode(
    const QDomNode &parent, const QString &tagName, const std::function<void(const QDomNode &)> &functor)
{
    callIfNodeExist(parent, tagName,             //
        [this, &functor](const QDomNode &node) { //
            callForEachChild(node, functor);     //
        });
}

template <> double XmlParse::parseNumString(const QString &numStr, bool &state)
{
    return numStr.toDouble(&state);
}

template <> int XmlParse::parseNumString(const QString &numStr, bool &state)
{
    return numStr.toInt(&state);
}

template <> quint16 XmlParse::parseNumString(const QString &numStr, bool &state)
{
    return numStr.toUShort(&state);
}

template <> quint32 XmlParse::parseNumString(const QString &numStr, bool &state)
{
    return numStr.toUInt(&state);
}

template <> quint64 XmlParse::parseNumString(const QString &numStr, bool &state)
{
    return numStr.toULongLong(&state);
}
