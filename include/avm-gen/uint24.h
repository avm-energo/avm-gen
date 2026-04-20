#pragma once

#include <avm-gen/gen_export.h>
#include <avm-gen/integers.h>
#include <cstdint>

constexpr unsigned int UINT24_MAX = 16777215;

class GENLIB_EXPORT uint24 final
{
private:
    constexpr inline static std::size_t elements = 3;
    u8 value[elements];

public:
    /// \brief Default-constructs a uint24 with value 0.
    uint24() noexcept;

    /// \brief Copy constructor.
    explicit uint24(const uint24 &val) noexcept;

    /// \brief Constructs from a 32-bit unsigned value (high byte is discarded).
    explicit uint24(const u32 val) noexcept;

    /// \brief Copy-assignment operator.
    uint24 &operator=(const uint24 &input) noexcept;

    /// \brief Assigns a 32-bit value, storing only the lowest 3 bytes.
    uint24 &operator=(const u32 input) noexcept;

    /// \brief Converts to u32 (zero-extended; no sign extension for an unsigned type).
    operator u32() const noexcept;

    /// \brief Converts to u16 by returning only the two low bytes.
    operator u16() const noexcept;

    // ---- Arithmetic operators (all delegate through u32 and truncate back to 24 bits) ----
    /// \brief Addition — result is truncated to 24 bits (overflow wraps modulo 2^24).
    uint24 operator+(const uint24 &val) const noexcept;

    /// \brief Subtraction — result is truncated to 24 bits (underflow wraps modulo 2^24).
    uint24 operator-(const uint24 &val) const noexcept;

    /// \brief Multiplication — result is truncated to the low 24 bits.
    uint24 operator*(const uint24 &val) const noexcept;

    /*! \brief Division.
     *  \warning Dividing by zero is undefined behaviour — the caller must ensure val != 0.
     */
    uint24 operator/(const uint24 &val) const noexcept;

    /// \brief Addition-assignment.
    uint24 &operator+=(const uint24 &val) noexcept;

    /// \brief Subtraction-assignment.
    uint24 &operator-=(const uint24 &val) noexcept;

    /// \brief Multiplication-assignment.
    uint24 &operator*=(const uint24 &val) noexcept;

    /// \brief Division-assignment.
    uint24 &operator/=(const uint24 &val) noexcept;

    // ---- Boolean / comparison operators ----
    /// \brief Returns true when the value is non-zero.
    operator bool() const noexcept;

    /// \brief Returns true when the value is zero.
    bool operator!() const noexcept;

    /// \brief Equality comparison.
    bool operator==(const uint24 &val) const noexcept;

    /// \brief Inequality comparison.
    bool operator!=(const uint24 &val) const noexcept;

    /// \brief Greater-than-or-equal comparison.
    bool operator>=(const uint24 &val) const noexcept;

    /// \brief Less-than-or-equal comparison.
    bool operator<=(const uint24 &val) const noexcept;
};
