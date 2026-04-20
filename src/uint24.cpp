#include <avm-gen/uint24.h>
#include <cstring> // memcpy

uint24::uint24() noexcept : value { 0, 0, 0 }
{
}

uint24::uint24(const uint24 &val) noexcept
{
    *this = val;
}

uint24::uint24(const u32 val) noexcept
{
    *this = val;
}

uint24 &uint24::operator=(const uint24 &input) noexcept
{
    value[0] = input.value[0];
    value[1] = input.value[1];
    value[2] = input.value[2];
    return *this;
}

uint24 &uint24::operator=(const u32 input) noexcept
{
    memcpy(value, &input, 3);
    return *this;
}

uint24::operator u32() const noexcept
{
    return (static_cast<u32>(value[2]) << 16) | (static_cast<u32>(value[1]) << 8) | static_cast<u32>(value[0]);
}

uint24::operator u16() const noexcept
{
    return static_cast<u16>((value[1] << 8) | value[0]);
}

uint24 uint24::operator+(const uint24 &val) const noexcept
{
    return uint24((u32) * this + (u32)val);
}

uint24 uint24::operator-(const uint24 &val) const noexcept
{
    return uint24((u32) * this - (u32)val);
}

uint24 uint24::operator*(const uint24 &val) const noexcept
{
    return uint24((u32) * this * (u32)val);
}

uint24 uint24::operator/(const uint24 &val) const noexcept
{
    return uint24((u32) * this / (u32)val);
}

uint24 &uint24::operator+=(const uint24 &val) noexcept
{
    *this = *this + val;
    return *this;
}

uint24 &uint24::operator-=(const uint24 &val) noexcept
{
    *this = *this - val;
    return *this;
}

uint24 &uint24::operator*=(const uint24 &val) noexcept
{
    *this = *this * val;
    return *this;
}

uint24 &uint24::operator/=(const uint24 &val) noexcept
{
    *this = *this / val;
    return *this;
}

uint24::operator bool() const noexcept
{
    return static_cast<u32>(*this) != 0;
}

bool uint24::operator!() const noexcept
{
    return !(static_cast<u32>(*this));
}

bool uint24::operator==(const uint24 &val) const noexcept
{
    return static_cast<u32>(*this) == static_cast<u32>(val);
}

bool uint24::operator!=(const uint24 &val) const noexcept
{
    return static_cast<u32>(*this) != static_cast<u32>(val);
}

bool uint24::operator>=(const uint24 &val) const noexcept
{
    return static_cast<u32>(*this) >= static_cast<u32>(val);
}

bool uint24::operator<=(const uint24 &val) const noexcept
{
    return static_cast<u32>(*this) <= static_cast<u32>(val);
}
