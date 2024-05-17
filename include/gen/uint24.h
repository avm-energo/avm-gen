#pragma once

#include <cstdint>
#include <gen/gen_export.h>
#include <gen/integers.h>

constexpr unsigned int UINT24_MAX = 16777215;

class GENLIB_EXPORT uint24 final
{
private:
    constexpr inline static std::size_t elements = 3;
    u8 value[elements];

public:
    uint24() noexcept;
    explicit uint24(const uint24 &val) noexcept;
    explicit uint24(const u32 val) noexcept;

    uint24 &operator=(const uint24 &input) noexcept;
    uint24 &operator=(const u32 input) noexcept;

    operator u32() const noexcept;
    operator u16() const noexcept;

    uint24 operator+(const uint24 &val) const noexcept;
    uint24 operator-(const uint24 &val) const noexcept;
    uint24 operator*(const uint24 &val) const noexcept;
    uint24 operator/(const uint24 &val) const noexcept;
    uint24 &operator+=(const uint24 &val) noexcept;
    uint24 &operator-=(const uint24 &val) noexcept;
    uint24 &operator*=(const uint24 &val) noexcept;
    uint24 &operator/=(const uint24 &val) noexcept;

    operator bool() const noexcept;
    bool operator!() const noexcept;
    bool operator==(const uint24 &val) const noexcept;
    bool operator!=(const uint24 &val) const noexcept;
    bool operator>=(const uint24 &val) const noexcept;
    bool operator<=(const uint24 &val) const noexcept;
};
