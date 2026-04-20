#pragma once
#include <QMap>
#include <QObject>
#include <avm-gen/utils/convertable.h>

class GENLIB_EXPORT MemSettings : public QObject
{
    Q_OBJECT
public:
    /*! \brief Constructs MemSettings and inserts a sentinel "default" key with value 0.
     *  \note The set() call during construction emits settingHasBeenChanged(), but no
     *        slots can be connected yet, so the signal is effectively a no-op here.
     */
    explicit MemSettings(QObject *parent = nullptr);

    /*! \brief Stores \a value under \a key and emits settingHasBeenChanged().
     *  \details Overwrites any previous value for the same key.
     */
    void set(const QString &key, const QVariant &value);

    /*! \brief Returns the value stored under \a key, or a null QVariant if not found.
     */
    utils::Convertable get(const QString &key) const;

    template <typename T> T get(const QString &name) const
    {
        return static_cast<T>(get(name));
    }

signals:
    void settingHasBeenChanged(const QString &name, const QVariant &value);

private:
    QMap<QString, QVariant> m_settingsMap;
};
