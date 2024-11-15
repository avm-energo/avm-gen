#include <gen/xmlfunc.h>

XmlFunc::XmlFunc()
{
}

// check attrname to contain attrs.at(0) in it and if so
// replace all attrs oldvalues with newvalues

void XmlFunc::replaceDomWithNewAttrRecursively(QByteArray &data, const QString &name, const QString &attrname,
    const QStringList &attrs, const QString &attrcheckvalue, const QStringList &newvalues)
{
    QDomDocument doc;
    doc.setContent(data);
    // recurivelly change color
    QDomElement root = doc.documentElement();

    replaceDomWithNewAttrRecursively(root, name, attrname, attrs, attrcheckvalue, newvalues);
}

void XmlFunc::replaceDomWithNewAttrRecursively(QDomElement &root, const QString &name, const QString &attrname,
    const QStringList &attrs, const QString &attrcheckvalue, const QStringList &newvalues)
{
    assert(attrs.size() == newvalues.size());
    QDomNodeList nodes = root.childNodes();
    for (int i = 0; i < nodes.size(); ++i)
    {
        QDomNode node = nodes.at(i);
        //        QString tmps = node.nodeName();
        if (node.nodeName() == name) // path
        {
            QDomNode newnode = node;
            QDomElement element = newnode.toElement();
            QString tmps = element.attribute(attrname); // style
            int count = 0;
            QString attrvalue = getAttrValue(tmps, attrs.first());
            if (attrvalue == attrcheckvalue)
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
                element.setAttribute(attrname, tmps);
                root.replaceChild(newnode, node);
            }
        }
        else
        {
            QDomElement element = node.toElement();
            replaceDomWithNewAttrRecursively(element, name, attrname, attrs, attrcheckvalue, newvalues);
        }
    }
}

QString XmlFunc::getAttrValue(const QString &string, const QString &attrname)
{
    QString tmps;
    int index = string.indexOf(attrname);
    if (index == -1)
        return QString();
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
    return QString();
}
