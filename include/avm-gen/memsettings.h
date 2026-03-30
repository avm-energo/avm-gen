#pragma once
#include <QMap>
#include <QObject>
#include <avm-gen/utils/convertable.h>

class GENLIB_EXPORT MemSettings : public QObject
{
    Q_OBJECT
public:
    explicit MemSettings(QObject *parent = nullptr);

    void set(const QString &key, const QVariant &value);
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
