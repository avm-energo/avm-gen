#pragma once

#include <QByteArray>
#include <gen/std_ext.h>
#include <vector>

namespace Files
{

/// \brief Zero-overhead представление структурированных бинарных файлов.
template <typename BinaryRecord> //
class BinaryFile
{
public:
    typedef std_ext::remove_cvref_t<BinaryRecord> value_type;
    typedef value_type *iterator;
    typedef const value_type *const_iterator;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    /// \brief Структура для представления диапазона итерируемых данных.
    template <typename Iterator> //
    struct Range
    {
        Iterator begin;
        Iterator end;

        inline operator bool() const noexcept
        {
            return (end - begin) > 0;
        }
    };

private:
    QByteArray m_file;
    static constexpr inline auto s_recordSize = sizeof(value_type);

    /// \brief Функция для проверки валидности внутренних данных.
    inline void checking(const char *where) const noexcept
    {
        Q_ASSERT_X(m_file.size() % s_recordSize == 0, where, "Incorrect file size");
        Q_UNUSED(where)
    }

public:
    explicit BinaryFile() noexcept = default;

    explicit inline BinaryFile(const QByteArray &file) noexcept : m_file(file)
    {
        checking("BinaryFile c-tor calling");
    }

    explicit inline BinaryFile(QByteArray &&file) noexcept : m_file(std::move(file))
    {
        checking("BinaryFile c-tor calling");
    }

    /// \brief Возвращает количество элементов типа BinaryRecord, хранимых в бинарном файле.
    inline size_type size() const noexcept
    {
        return m_file.size() / s_recordSize;
    }

    /// \brief Возвращает итератор на начало бинарного файла.
    inline iterator begin() noexcept
    {
        return reinterpret_cast<iterator>(m_file.data());
    }

    /// \brief Возвращает итератор на конец бинарного файла.
    inline iterator end() noexcept
    {
        return (begin() + size());
    }

    /// \brief Возвращает константный итератор на начало бинарного файла.
    inline const_iterator begin() const noexcept
    {
        return reinterpret_cast<const_iterator>(m_file.constData());
    }

    /// \brief Возвращает константный итератор на конец бинарного файла.
    inline const_iterator end() const noexcept
    {
        return (begin() + size());
    }

    /// \brief Возвращает ссылку на первый элемент типа
    /// BinaryRecord, хранимого в бинарном файле.
    inline reference first() noexcept
    {
        return *begin();
    }

    /// \brief Возвращает константную ссылку на первый элемент
    /// типа BinaryRecord, хранимого в бинарном файле.
    inline const_reference first() const noexcept
    {
        return *begin();
    }

    /// \brief Возвращает ссылку на последний элемент типа
    /// BinaryRecord, хранимого в бинарном файле.
    inline reference last() noexcept
    {
        return *(begin() + (size() - 1));
    }

    /// \brief Возвращает константную ссылку на последний элемент
    /// типа BinaryRecord, хранимого в бинарном файле.
    inline const_reference last() const noexcept
    {
        return *(begin() + (size() - 1));
    }

    /// \brief Обновляет содержимое объекта новым бинарным файлом.
    inline void update(const QByteArray &newFile) noexcept
    {
        m_file = newFile;
        checking("update calling");
    }

    /// \brief Возвращает копию текущего бинарного файла как
    /// хранилище объектов типа AnotherBinaryRecord.
    template <typename AnotherBinaryRecord> //
    inline BinaryFile<AnotherBinaryRecord> copyAs() const noexcept
    {
        return BinaryFile<AnotherBinaryRecord>(m_file);
    }

    /// \brief Возвращает первый найденный диапазон элементов, для
    /// которых переданный унарный предикат возвращает true.
    /// \param first[in] - итератор на начало поиска диапазона.
    /// \param last[in] - итератор на конец поиска диапазона.
    /// \param predicate[in] - унарный предикат, принимающий константную
    /// ссылку на элемент и возвращающий true или false в зависимости от внутренней логики.
    template <typename Iterator, typename Predicate> //
    static Range<Iterator> findRange(Iterator first, Iterator last, Predicate predicate) noexcept
    {
        bool firstFound = false;
        Range<Iterator> range { last, last };
        for (Iterator iter = first; iter != last; ++iter)
        {
            auto predicateResult { predicate(*iter) };
            if (predicateResult && !firstFound)
            {
                range.begin = iter;
                firstFound = true;
            }
            else if (!predicateResult && firstFound)
            {
                range.end = iter;
                break;
            }
        }
        return range;
    }

    /// \brief Возвращает динамический массив из всех найденных диапазонов
    /// элементов, для которых переданный унарный предикат возвращает true.
    /// \see findRange
    template <typename Iterator, typename Predicate> //
    static std::vector<Range<Iterator>> findAllRanges(Iterator first, Iterator last, Predicate predicate) noexcept
    {
        std::vector<Range<Iterator>> ranges;
        ranges.reserve(8);
        while (first != last)
        {
            Range<Iterator> range { findRange(first, last, predicate) };
            if (!range)
                break;
            else
            {
                ranges.push_back(range);
                first = range.end;
            }
        }
        return ranges;
    }

    template <typename UnaryPredicate> //
    inline Range<iterator> findRange(UnaryPredicate p) noexcept
    {
        return BinaryFile<BinaryRecord>::findRange(begin(), end(), p);
    }

    template <typename UnaryPredicate> //
    inline Range<const_iterator> findRange(UnaryPredicate p) const noexcept
    {
        return BinaryFile<BinaryRecord>::findRange(begin(), end(), p);
    }

    template <typename UnaryPredicate> //
    inline std::vector<Range<iterator>> findAllRanges(UnaryPredicate p) noexcept
    {
        return BinaryFile<BinaryRecord>::findAllRanges(begin(), end(), p);
    }

    template <typename UnaryPredicate> //
    inline std::vector<Range<const_iterator>> findAllRanges(UnaryPredicate p) const noexcept
    {
        return BinaryFile<BinaryRecord>::findAllRanges(begin(), end(), p);
    }

    /// \brief Удаляет из бинарного файла все записи в интервале [first, last).
    inline void remove(iterator first, iterator last) noexcept
    {
        Q_ASSERT(last >= first);
        difference_type index = (first - begin()) * s_recordSize;
        difference_type length = (last - first) * s_recordSize;
        m_file.remove(index, length);
        checking("remove calling");
    }

    /// \brief Удаляет из бинарного файла все записи в переданном диапазоне.
    inline void remove(const Range<iterator> &range) noexcept
    {
        remove(range.begin, range.end);
    }

    /// \brief Перемещает все записи из интервала [first, last) на
    /// позицию перед элементом, на который указывает итератор dst.
    [[nodiscard]] inline bool move(iterator srcBegin, iterator srcEnd, iterator dst) noexcept
    {
        // Проверки на валидность диапазона
        difference_type length = (srcEnd - srcBegin) * s_recordSize;
        difference_type index = (dst - begin()) * s_recordSize;
        index = ((dst - srcBegin) > 0) ? (index - length) : index;
        if ((srcBegin == begin() && srcEnd == end()) ||                //
            (dst >= srcBegin && dst <= srcEnd) || (dst == srcBegin) || //
            (dst == srcEnd) || (length <= 0) || (index < 0) ||         //
            (dst < begin()) || (dst > end()))
            return false;

        QByteArray tempRange(length, 0);
        std::copy(srcBegin, srcEnd, reinterpret_cast<iterator>(tempRange.data()));
        remove(srcBegin, srcEnd);
        m_file.insert(index, tempRange);
        checking("move calling");
        return true;
    }

    /// \brief Перемещает все записи из переданного диапазона на
    /// позицию перед элементом, на который указывает итератор dst.
    [[nodiscard]] inline bool move(const Range<iterator> &range, iterator dst) noexcept
    {
        return move(range.begin, range.end, dst);
    }
};

} // namespace Files
