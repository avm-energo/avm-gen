#include <avm-gen/netip.h>

const NetIP IP::DefIP()
{
    return { 0, 0, 0, 0 };
}

const NetIP IP::fromString(const QString &str)
{
    return fromStringList(str.split("."));
}

const NetIP IP::fromStringList(const QStringList &sl)
{
    NetIP ip;
    if (sl.size() < 4)
        return DefIP();
    else
    {
        for (int i = 0; i < 4; ++i)
        {
            bool ok;
            int ipOctet = sl.at(i).toUInt(&ok);
            ip.at(i) = (ok && (ipOctet < 255)) ? ipOctet : 0;
        }
    }
    return ip;
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
