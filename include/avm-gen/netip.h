#pragma once

#include <QObject>
#include <array>
#include <avm-gen/gen_export.h>

static constexpr auto NET_IP_SIZE = 4; ///< число октетов IP адресе
using NetIP = std::array<quint8, NET_IP_SIZE>;

namespace IP {
const GENLIB_EXPORT NetIP DefIP();
const GENLIB_EXPORT NetIP fromString(const QString &str);
const GENLIB_EXPORT NetIP fromStringList(const QStringList &sl);
const GENLIB_EXPORT QString toString(const NetIP &ip);
const GENLIB_EXPORT QStringList toStringList(const NetIP &ip);
}; // namespace IP
