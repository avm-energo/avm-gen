#pragma once

#include <type_traits>
#include <typeinfo>
#include <utility>
#include <variant>

// idea of https://stackoverflow.com/a/24687161/8524139

template <bool... Bs> //
using bool_sequence = std::integer_sequence<bool, Bs...>;

template <bool... Bs> //
using bool_and = std::is_same<bool_sequence<Bs...>, bool_sequence<(Bs || true)...>>;

template <bool... Bs> //
using bool_or = std::integral_constant<bool, !bool_and<!Bs...>::value>;

namespace std_ext
{
template <typename Enum> //
constexpr std::underlying_type_t<Enum> to_underlying(Enum value) noexcept
{
    return static_cast<std::underlying_type_t<Enum>>(value);
}

template <typename T, typename = void> //
struct is_container : std::false_type
{
};

template<typename... Ts>
struct is_container_helper
{};

template<typename T>
struct is_container<T,
                    std::conditional_t<false,
                                       is_container_helper<typename T::value_type,
                                                           typename T::size_type,
                                                           typename T::iterator,
                                                           typename T::const_iterator,
                                                           decltype(std::declval<T>().size()),
                                                           decltype(std::declval<T>().begin()),
                                                           decltype(std::declval<T>().end()),
                                                           decltype(std::declval<T>().cbegin()),
                                                           decltype(std::declval<T>().cend())>,
                                       void>> : public std::true_type
{};

template<typename T, typename Variant, std::size_t I = std::variant_size<Variant>::value>
static constexpr bool is_variant_alternative()
{
    if constexpr (I > 0) {
        constexpr auto index = I - 1;
        if constexpr (std::is_same_v<T, std::variant_alternative_t<index, Variant>>)
            return true;
        else
            return is_variant_alternative<T, Variant, I - 1>();
    } else
        return false;
}

template <class Variant> //
std::type_info const &variant_type(Variant const &v)
{
    return std::visit([](auto &&x) -> decltype(auto) { return typeid(x); }, v);
}

#ifndef __cpp_lib_remove_cvref
template <class T> //
struct remove_cvref
{
    using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template <class T> //
using remove_cvref_t = typename remove_cvref<T>::type;
#else // ^^^ __cpp_lib_remove_cvref / !__cpp_lib_remove_cvref vvv

#endif // __cpp_lib_remove_cvref

#if defined(Q_CC_MSVC)
__forceinline
#elif defined(Q_CC_GNU) || defined(Q_CC_CLANG)
__attribute__((always_inline)) inline
#else
inline
#endif
    unsigned int
    clp2(unsigned int x)
{
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return ++x;
}

}

// helper type for the visitor
template <class... Ts> //
struct overloaded : Ts...
{
    using Ts::operator()...;
};

// explicit deduction guide (not needed as of C++20)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
