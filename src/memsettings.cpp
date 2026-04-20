#include <avm-gen/memsettings.h>

MemSettings::MemSettings(QObject *parent) : QObject { parent }
{
    set("default", 0);
}

void MemSettings::set(const QString &key, const QVariant &value)
{
    m_settingsMap[key] = value;
    emit settingHasBeenChanged(key, value);
}

utils::Convertable MemSettings::get(const QString &key) const
{
    return utils::Convertable(m_settingsMap.value(key));
}
