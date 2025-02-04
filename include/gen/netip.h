#pragma once

#include <QObject>
#include <array>

static constexpr auto NET_IP_SIZE = 4; ///< число октетов IP адресе
using NetIP = std::array<quint8, NET_IP_SIZE>;
