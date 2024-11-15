#include <gen/xmlfunc.h>

XmlFunc::XmlFunc()
{
}

// check attrname to contain attrs.at(0) in it and if so
// replace all attrs oldvalues with newvalues

void XmlFunc::replaceDomWithNewAttrRecursively(
    QByteArray &data, AttrsSearchedStruct &findInfo, const QStringList &attrs, const QStringList &newvalues)
{
    QDomDocument doc;
    doc.setContent(data);
    // recurivelly change color
    QDomElement root = doc.documentElement();

    replaceDomWithNewAttrRecursively(root, findInfo, attrs, newvalues);
    data = doc.toByteArray();
}

void XmlFunc::replaceDomWithNewAttrRecursively(
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

QString XmlFunc::getAttrValue(QString &string, const QString &attrname)
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
        string = string.sliced(index);
    }
    return QString();
}
