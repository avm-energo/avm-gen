#include "gen/utils/convertable.h"

namespace utils
{

Convertable::Convertable(const QVariant &data) noexcept : m_data(data)
{
}

Convertable::Convertable(QVariant &&data) noexcept : m_data(std::move(data))
{
}

Convertable::Convertable(const Convertable &rhs) noexcept : m_data(rhs.m_data)
{
}

Convertable::Convertable(Convertable &&rhs) noexcept : m_data(std::move(rhs.m_data))
{
}

Convertable &Convertable::operator=(const Convertable &rhs) noexcept
{
    m_data = rhs.m_data;
    return *this;
}

Convertable &Convertable::operator=(Convertable &&rhs) noexcept
{
    m_data = std::move(rhs.m_data);
    return *this;
}

QVariant &Convertable::operator*() noexcept
{
    return m_data;
}

const QVariant &Convertable::operator*() const noexcept
{
    return m_data;
}

Convertable::operator bool() const noexcept
{
    return m_data.toBool();
}

Convertable::operator QString() const noexcept
{
    return m_data.toString();
}

Convertable::operator QByteArray() const noexcept
{
    return m_data.toByteArray();
}

Convertable::operator std::int16_t() const noexcept
{
    return static_cast<std::int16_t>(m_data.toInt());
}

Convertable::operator std::uint16_t() const noexcept
{
    return static_cast<std::uint16_t>(m_data.toUInt());
}

Convertable::operator std::int32_t() const noexcept
{
    return static_cast<std::int32_t>(m_data.toInt());
}

Convertable::operator std::uint32_t() const noexcept
{
    return static_cast<std::uint32_t>(m_data.toUInt());
}

Convertable::operator std::int64_t() const noexcept
{
    return static_cast<std::int64_t>(m_data.toLongLong());
}

Convertable::operator std::uint64_t() const noexcept
{
    return static_cast<std::uint64_t>(m_data.toULongLong());
}

} // namespace utils
