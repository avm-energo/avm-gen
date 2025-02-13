#pragma once

#include <QObject>
#include <array>

static constexpr auto NET_IP_SIZE = 4; ///< число октетов IP адресе
using NetIP = std::array<quint8, NET_IP_SIZE>;

class IP
{
public:
    explicit IP();
    explicit IP(const QString &str);
    explicit IP(const QStringList &sl);
    const QString toString();
    const QStringList toStringList();

private:
    NetIP m_ip;
};
