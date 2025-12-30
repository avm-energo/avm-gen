#include <avm-gen/uint24.h>

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
    value[0] = ((unsigned char *)&input)[0];
    value[1] = ((unsigned char *)&input)[1];
    value[2] = ((unsigned char *)&input)[2];
    return *this;
}

uint24::operator u32() const noexcept
{
    /* Sign extend negative quantities */
    if (value[2] & 0x80)
        return (0xff << 24) | (value[2] << 16) | (value[1] << 8) | value[0];
    else
        return (value[2] << 16) | (value[1] << 8) | value[0];
}

uint24::operator u16() const noexcept
{
    return (value[1] << 8) | value[0];
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
