#include <gen/netip.h>

IP::IP()
{
    m_ip = { 0, 0, 0, 0 };
}

IP::IP(const QString &str)
{
    IP(str.split("."));
}

IP::IP(const QStringList &sl)
{
    if (sl.size() < 4)
        IP();
    else
    {
        for (int i = 0; i < 4; ++i)
        {
            bool ok;
            int ipOctet = sl.at(i).toUInt(&ok);
            m_ip.at(i) = (ok && (ipOctet < 255)) ? ipOctet : 0;
        }
    }
}

const QString IP::toString()
{
    return toString(m_ip);
}

const QStringList IP::toStringList()
{
    return toStringList(m_ip);
}

const QString IP::toString(const NetIP &ip)
{
    QString outs = "";
    for (quint8 octet : ip)
    {
        outs += QString::number(octet);
        outs += ".";
    }
    outs.chop(1);
    return outs;
}

const QStringList IP::toStringList(const NetIP &ip)
{
    QStringList outsl;
    for (quint8 octet : ip)
        outsl += QString::number(octet);
    return outsl;
}
