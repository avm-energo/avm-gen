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
    QString outs = "";
    for (quint8 octet : m_ip)
    {
        outs += QString::number(octet);
        outs += ".";
    }
    outs.chop(1);
    return outs;
}

const QStringList IP::toStringList()
{
    QStringList outsl;
    for (quint8 octet : m_ip)
        outsl += QString::number(octet);
    return outsl;
}
