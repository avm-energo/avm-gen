#pragma once

#include <QDomNode>
#include <QObject>
#include <QtDebug>

class XmlParse
{
public:
    XmlParse();

    /// \brief Проверка, существует ли указанный файл.
    bool isFileExist(const QString &filename);

    /// \brief Парсит ноду с дочерними элементами в QStringList.
    static const QStringList parseArray(const QDomNode &node, const QString &tag);
    /// \brief Парсинг значений указанного типа в из ноды tag в QList.
    template <typename T> static const QList<T> parseNumArray(const QDomNode &node, const QString &tag)
    {
        QList<quint32> retList = {};
        bool state = false;
        for (const auto &val : parseArray(node, tag))
        {
            auto number = parseNumString<T>(val, state);
            if (state)
                retList.append(number);
            else
                retList.append(T(0));
        }
        return retList;
    };

    /// \brief Парсинг содержимого узла в число указанного типа.
    template <typename T> static T parseNum(const QDomElement &numNode)
    {
        auto numString = numNode.text();
        if (!numString.isEmpty())
        {
            auto state = false;
            auto number = parseNumString<T>(numString, state);
            if (state)
                return number;
            else
                qWarning() << "Parsing number failed!";
        }
        return T(0);
    }

    /// \brief Нахождение узла с указанным именем и парсинг его содержимого, возврат числа.
    template <typename T> static T parseNumFromNode(const QDomNode &node, const QString &tagName)
    {
        auto numNode = node.firstChildElement(tagName);
        if (!numNode.isNull())
            return parseNum<T>(numNode);
        else
            return T(0);
    }

    /// \brief Возвращает содержимое ноды tagName в QString.
    const QString parseString(const QDomNode &node, const QString &tagName) const;
    /// \brief Callback для вызова функции functor, если у указанного
    /// узла parent существует дочерний узел с именем tagName.
    void callIfNodeExist(const QDomNode &parent, const QString &tagName, //
        const std::function<void(const QDomNode &node)> &functor);
    /// \brief Callback для вызова функции functor, для каждого
    /// дочернего узла node указанного узла parent.
    void callForEachChild(const QDomNode &parent, const std::function<void(const QDomNode &node)> &functor);
    /// \brief Вызов callForEachChild внутри callIfNodeExist для функции functor.
    /// \see callIfNodeExist, callForEachChild.
    void parseNode(const QDomNode &parent, const QString &tagName, //
        const std::function<void(const QDomNode &node)> &functor);

    /// \brief Парсинг содержимого строки в число.
    template <typename T> static T parseNumString(const QString &numStr, bool &state);
};

// Template specializations
/// \brief Template specialization for converting QString to double.
template <> double XmlParse::parseNumString(const QString &numStr, bool &state);
/// \brief Template specialization for converting QString to int.
template <> int XmlParse::parseNumString(const QString &numStr, bool &state);
/// \brief Template specialization for converting QString to uint16.
template <> quint16 XmlParse::parseNumString(const QString &numStr, bool &state);
/// \brief Template specialization for converting QString to uint32.
template <> quint32 XmlParse::parseNumString(const QString &numStr, bool &state);
/// \brief Template specialization for converting QString to uint64.
template <> quint64 XmlParse::parseNumString(const QString &numStr, bool &state);
