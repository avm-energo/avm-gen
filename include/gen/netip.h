#pragma once

#include <QObject>
#include <array>

static constexpr auto NET_IP_SIZE = 4; ///< число октетов IP адресе
using NetIP = std::array<quint8, NET_IP_SIZE>;

namespace IP
{
const NetIP DefIP();
const NetIP fromString(const QString &str);
const NetIP fromStringList(const QStringList &sl);
const QString toString(const NetIP &ip);
const QStringList toStringList(const NetIP &ip);
};
