#include <avm-gen/xml/xmlattr.h>
#include <avm-gen/xml/xmlbase.h>

XmlAttr::XmlAttr()
{
}

// check attrname to contain attrs.at(0) in it and if so
// replace all attrs oldvalues with newvalues

bool XmlAttr::replaceComplicatedAttrRecursively(
    QByteArray &data, AttrsSearchedStruct &findInfo, const QStringList &attrs, const QStringList &newvalues)
{
    QDomDocument doc;
    doc.setContent(data);
    // recurivelly change color
    QDomElement root;
    if (!XmlBase::getXMLFromByteArray(data, root))
        return false;
    replaceDomWithNewAttrRecursively(root, findInfo, attrs, newvalues);
    data = XmlBase::toByteArray();
    return true;
}

bool XmlAttr::replaceSimpleAttr(
    QByteArray &data, const QString &elementName, QStringList &attrs, QStringList &newvalues)
{
    QDomElement root;
    if (!XmlBase::getXMLFromByteArray(data, root))
        return false;
    assert(attrs.size() == newvalues.size());
    assert(!attrs.isEmpty());
    // recurivelly change color
    QDomNodeList nodes = root.childNodes();
    for (int i = 0; i < nodes.size(); ++i)
    {
        QDomNode node = nodes.at(i);
        if (node.nodeName() == elementName)
        {
            QDomNode newnode = node;
            QDomElement element = newnode.toElement();
            while ((!attrs.isEmpty()) && (!newvalues.isEmpty()))
            {
                const QString attr = attrs.takeFirst();
                const QString value = newvalues.takeFirst();
                if (element.hasAttribute(attr))
                    element.setAttribute(attr, value);
            }
            root.replaceChild(newnode, node);
        }
    }
    data = XmlBase::toByteArray();
    return true;
}

void XmlAttr::replaceDomWithNewAttrRecursively(
    QDomElement &root, AttrsSearchedStruct &findInfo, const QStringList &attrs, const QStringList &newvalues)
{
    assert(attrs.size() == newvalues.size());
    QDomNodeList nodes = root.childNodes();
    for (int i = 0; i < nodes.size(); ++i)
    {
        QDomNode node = nodes.at(i);
        //        QString tmps = node.nodeName();
        if (node.nodeName() == findInfo.DomElementName) // path
        {
            QDomNode newnode = node;
            QDomElement element = newnode.toElement();
            QString tmps = element.attribute(findInfo.AttrName); // style
            int count = 0;
            QString attrvalue = getAttrValue(tmps, findInfo.ParameterName);
            if (attrvalue == findInfo.ParameterValue)
            {
                while (!attrvalue.isEmpty())
                {
                    if (count < newvalues.size())
                    {
                        tmps = tmps.replace(attrvalue, newvalues.at(count));
                        ++count;
                        if (count < attrs.size())
                            attrvalue = getAttrValue(tmps, attrs.at(count));
                        else
                            attrvalue.clear();
                    }
                }
                element.setAttribute(findInfo.AttrName, tmps);
                root.replaceChild(newnode, node);
            }
        }
        else
        {
            QDomElement element = node.toElement();
            replaceDomWithNewAttrRecursively(element, findInfo, attrs, newvalues);
        }
    }
}

QString XmlAttr::getAttrValue(QString &string, const QString &attrname)
{
    QString tmps;
    int index;
    while ((index = string.indexOf(attrname)) != -1)
    {
        index += attrname.size();
        if (index >= string.size())
            return QString();
        if (string.at(index++) == ':')
        {
            while ((index < string.size()) && (string.at(index) == ' '))
                index++;
            while ((index < string.size()) && (string.at(index) != ';') && (string.at(index) != '\"'))
            {
                tmps += string.at(index++);
            }
            return tmps;
        }
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        string = string.mid(index);
#else
        string = string.sliced(index);
#endif
    }
    return QString();
}
