#pragma once

#include <QObject>
#include <QString>
#include <avm-gen/gen_export.h>
#include <avm-gen/uint24.h>
#include <memory>
#include <type_traits>

#define MAINSLEEP 20
#define MAINTIMEOUT 2000 // 2 sec timeout for general operations

/// \brief Namespace with internal helper structure
namespace QtHelper
{
/*! \brief Functor for auto-deleting
 *  \details Functor for auto-deleting object, used in UniquePointer as default deletor
 */
struct deleteLaterDeletor
{
    void operator()(QObject *object) const
    {
        if (object)
        {
            object->deleteLater();
        }
    }
};
}

template <typename T> using SharedPointer = std::shared_ptr<T>;

template <typename T> using UniquePointer = std::unique_ptr<T, QtHelper::deleteLaterDeletor>;

/// \brief Декларация для определения простых (POD) типов.
/// \see https://en.cppreference.com/w/cpp/types/is_pod
template <typename T> //
constexpr static auto is_simple_v = std::is_standard_layout_v<T> && std::is_trivial_v<T>;

constexpr int defaultRatio = 3;
constexpr int maxRatio = 5;

/*! \brief Class contains most frequently used general purpose functions
 *  \details That class contains static general purpose functions
 */
class GENLIB_EXPORT StdFunc
{
private:
public:
    static struct
    {
        bool cancelled = false;
        bool cancelEnabled = true;
    } s_state;

    StdFunc() = default;

    /*! \brief Initialization function for static class fields.
     *  \details Initialize next fields by values: system home directory, organization, device IP, etc...
     */
    static void Init();

    /// \brief Converts a version from quint32 datatype to string view.
    static QString VerToStr(quint32);

    /// \brief Converts a version from string view to quint32 datatype.
    static quint32 StrToVer(const QString &str);

    /*! \brief Checks that the number is in the specified interval.
     *  \param var Checked number.
     *  \param base,tolerance Limits of specified interval.
     */
    static bool FloatIsWithinLimits(double var, double base, double tolerance);

    /// \brief Converts a value from string view to float point datatype.
    static float ToFloat(const QString &text, bool &ok);

    /*! \brief Converts \a value to a human-readable string.
     *  \param value      Float value to format.
     *  \param precision  Number of digits after the decimal point.
     *  \param exp        If true, uses scientific ('e') notation; otherwise fixed ('f').
     *  \return Formatted string, or "***" when value is infinite / exceeds MAXFLOAT.
     */
    static QString toString(float value, int precision = 5, bool exp = false);

    /*! \brief Returns the name of the OS user running this process.
     *  \details Runs the platform "whoami" command synchronously.
     *  \warning Spins on processEvents() until the process exits; avoid calling
     *           from a thread that does not have an event loop.
     */
    static QString WhoAmI();

    /// \brief Sets cancel s_state when enabled.
    static void Cancel();

    /// \brief Turns off cancel s_state.
    static void ClearCancel();

    /// \brief Returns cancel s_state.
    static bool IsCancelled();

    /// \brief Disallows to set cancel s_state.
    static void SetCancelDisabled();

    /// \brief Allows to set cancel s_state.
    static void SetCancelEnabled();

    /*! \brief Returns the 1-based position of the first set bit (LSB = position 1).
     *  \details Scans from bit 0 upward and returns the position of the first '1'.
     *  \param dword 32-bit bitstring to examine.
     *  \return 1-based index of the first set bit, or 0 if \a dword is 0.
     */
    static int IndexByBit(quint32 dword);

    /*! \brief Returns the 32bit bitstring by index position.
     *  \details Returns 32bit bitstring with '1' in index position.
     *  \param index Position of '1' from LSB.
     *  \return Example: 0 => 0, 1 => 1, 2 => 2, 3 => 4, ...
     */
    static quint32 BitByIndex(int idx);

    /// \brief Puts the thread to sleep for a given time in ms.
    static void Wait(int ms = MAINSLEEP);

    static inline int goldenRatio(int value)
    {
        int multiplier = value / 10;
        for (auto i = maxRatio + multiplier; i != defaultRatio; --i)
        {
            if (!(value % i))
                return i;
        }
        return defaultRatio + multiplier;
    }

    /*! \brief Ping IP address, return IP address if host is alive or return 0 if host is dead
     *  \details Platform dependent ping function, ping IP address through cmdline utility,
     *  parse cmd output. If output contains TTL host is alive else host is dead.
     *  \return IP address if host is alive, otherwise returns 0.
     */
    static quint32 Ping(quint32 addr);

    /*! \brief Checks port and IPv4 address for connection.
     *  \details Checks if the connection can be made with given IP address and port.
     *  \param ip4Addr[in] IPv4 host address.
     *  \param port[in] Connection port.
     *  \return IPv4 address if connection can be made, 0 otherwise.
     */
    static quint32 CheckPort(quint32 ip4Addr, quint16 port);

    /*! \brief Removes specified substring from specified string.
     *  \param str[in, out] The string from which the substring will be removed.
     *  \param substr[i] The substring that will be removed from string.
     */
    static void RemoveSubstr(std::string &str, std::string &substr);

    /*! \brief Template function for joining items into QList
     *  \param list[out] QList for joining input item
     *  \param item[in] Input item
     */
    template <typename T> static void joinItem(QList<T> &list, const T item)
    {
        list.append(item);
    }

    /// \brief Converts an instance of trivial data type to the byte array.
    /// \param value[in] Input data for convertation.
    template <typename T, std::size_t size = sizeof(T),
        std::enable_if_t<is_simple_v<T> || std::is_same_v<T, uint24>, bool> = true> //
    static QByteArray toByteArray(const T &value)
    {
        return QByteArray::fromRawData(reinterpret_cast<const char *>(&value), size);
    }

    /// \brief Converts an rvalue of trivial data type to the byte array.
    /// \param value[in] An rvalue of simple data type.
    template <typename T, std::size_t size = sizeof(T),
        std::enable_if_t<is_simple_v<T> || std::is_same_v<T, uint24>, bool> = true> //
    static QByteArray toByteArray(T &&value)
    {
        QByteArray retVal(size, 0);
        *reinterpret_cast<T *>(retVal.data()) = std::move(value);
        return retVal;
    }

    /// \brief Converts list of known datatype to QVariant list.
    template <typename T> static QVariantList ToVariantList(const QList<T> &list)
    {
        QVariantList newList;
        for (const T &item : list)
            newList.push_back(item);
        return newList;
    }

    /// \brief Converts QVariant list to list of pointers of known datatype.
    template <typename T> static QList<T> ToValuesList(const QList<T *> &list)
    {
        QList<T> newList;
        newList.reserve(list.size());
        std::transform(std::begin(list), std::end(list), std::back_inserter(newList), [](T *item) { return *item; });
        return newList;
    }

    /// \brief Safe memory copying from the a byte array to a given simple data type object.
    template <typename T, std::size_t count = sizeof(T), std::enable_if_t<is_simple_v<T>, bool> = true> //
    static inline void safeMemoryCopy(T &dst, const QByteArray &src, const std::size_t startIndex) noexcept
    {
        if (static_cast<std::size_t>(src.size()) >= startIndex + count)
        {
            auto dstBegin = reinterpret_cast<std::uint8_t *>(&dst);
            auto srcBegin = reinterpret_cast<const std::uint8_t *>(src.constData()) + startIndex;
            std::copy_n(srcBegin, count, dstBegin);
        }
    }

    /// \brief Returns an object of T type from a given byte array at a given index position.
    template <typename T, std::enable_if_t<is_simple_v<T>, bool> = true> //
    static inline T getFromByteArray(const QByteArray &bytes, const std::size_t startIndex = 0) noexcept
    {
        T value {};
        safeMemoryCopy(value, bytes, startIndex);
        return value;
    }
};
