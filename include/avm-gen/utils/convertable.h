#pragma once

#include <QVariant>
#include <avm-gen/gen_export.h>
#include <avm-gen/integers.h>

namespace utils
{

/// \brief Class-wrapper for any received QVariant with explicit defined type conversion operators.
class GENLIB_EXPORT Convertable
{
private:
    QVariant m_data;

public:
    /// \brief Parametrized c-tor with a const reference to the input data.
    explicit Convertable(const QVariant &data) noexcept;
    /// \brief Parametrized c-tor with rvalue input data.
    explicit Convertable(QVariant &&data) noexcept;
    /// \brief Copy c-tor.
    Convertable(const Convertable &rhs) noexcept;
    /// \brief Move c-tor.
    Convertable(Convertable &&rhs) noexcept;

    /// \brief Assignment operator overloading with a const reference.
    Convertable &operator=(const Convertable &rhs) noexcept;
    /// \brief Assignment operator overloading with a rvalue.
    Convertable &operator=(Convertable &&rhs) noexcept;

    /// \brief Indirection operator overloading.
    QVariant &operator*() noexcept;
    /// \brief Indirection operator overloading (for const type).
    const QVariant &operator*() const noexcept;

    operator QVariant() const noexcept;
    /// \brief Implicit type conversion to bool value.
    operator bool() const noexcept;
    /// \brief Implicit type conversion to string value.
    operator QString() const noexcept;

    operator QByteArray() const noexcept;

    operator i8() const noexcept;
    operator u8() const noexcept;
    operator i16() const noexcept;
    operator u16() const noexcept;
    operator i32() const noexcept;
    operator u32() const noexcept;
    operator i64() const noexcept;
    operator u64() const noexcept;
};

} // namespace utils
