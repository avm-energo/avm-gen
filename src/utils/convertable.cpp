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

Convertable::operator QVariant() const noexcept
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

Convertable::operator i8() const noexcept
{
    return static_cast<i8>(m_data.toInt());
}

Convertable::operator u8() const noexcept
{
    return static_cast<u8>(m_data.toUInt());
}

Convertable::operator i16() const noexcept
{
    return static_cast<i16>(m_data.toInt());
}

Convertable::operator u16() const noexcept
{
    return static_cast<u16>(m_data.toUInt());
}

Convertable::operator i32() const noexcept
{
    return static_cast<i32>(m_data.toInt());
}

Convertable::operator u32() const noexcept
{
    return static_cast<u32>(m_data.toUInt());
}

Convertable::operator i64() const noexcept
{
    return static_cast<i64>(m_data.toLongLong());
}

Convertable::operator u64() const noexcept
{
    return static_cast<u64>(m_data.toULongLong());
}

} // namespace utils
