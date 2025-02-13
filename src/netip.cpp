#include <gen/netip.h>

IP::IP()
{
    m_ip = { 0, 0, 0, 0 };
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
