#include "qsettingsmapper.h"
#include "qutils_log.h"

#include <QStandardPaths>

Q_GLOBAL_STATIC(QString, g_path)
Q_GLOBAL_STATIC(QString, g_name)

void QSettingsMapper::setDefaultPath(const QString& path)
{
    *g_path = path;
    QUTILSLOG_INFO()<<"QSettingsMapper default path set to:"<<path;
}

void QSettingsMapper::setDefaultName(const QString& name)
{
    *g_name = name;
    QUTILSLOG_INFO()<<"QSettingsMapper default name set to:"<<name;
}

const QString& QSettingsMapper::defaultPath()
{
    return *g_path;
}

const QString& QSettingsMapper::defaultName()
{
    return *g_name;
}

QSettingsMapper::QSettingsMapper(QObject *parent) :
    QSettingsMapper(*g_path, *g_name, "", parent)
{

}

QSettingsMapper::QSettingsMapper(const QString& baseName, QObject *parent) :
    QSettingsMapper(*g_path, baseName, "", parent)
{

}

QSettingsMapper::QSettingsMapper(const QString& settingsPath, const QString& baseName, QObject *parent) :
    QSettingsMapper(settingsPath, baseName, "", parent)
{

}

QSettingsMapper::QSettingsMapper(const QString& settingsPath, const QString& baseName, const QString& settingsCategory, QObject *parent) :
    QVariantMapper(baseName, parent, &QSettingsMapper::staticMetaObject),
    m_settingsPath(settingsPath),
    m_settingsCategory(settingsCategory)
{
    connect(this, &QSettingsMapper::settingsPathChanged, this, &QVariantMapper::queueSelect);
    connect(this, &QSettingsMapper::settingsCategoryChanged, this, &QVariantMapper::queueSelect);
}

QSettingsMapper::~QSettingsMapper()
{
    if(m_submitPolicy<QVariantMapperPolicies::Manual && m_isDirty) {
        submit();
        waitForSubmit();
    }
}

bool QSettingsMapper::doSelect()
{
    delete m_settings;

    bool result = true;
    const QVariantMap storage = selectSettings(&result);
    setStorage(storage);
    emitSelectDone(result);

    return true;
}

bool QSettingsMapper::doSubmit(const QStringList& dirtyKeys)
{
    bool result = submitSettings(dirtyKeys);
    emitSubmitDone(result);

    return result;
}

QVariantMap QSettingsMapper::selectSettings(bool* result)
{
    return fromFile(result);
}

QVariantMap QSettingsMapper::fromFile(bool* result)
{
    QVariantMap map;
    const QStringList settingsKeys = instance()->childKeys();
    for(const QString& key: settingsKeys)
    {
        if(getMappedKeys().isEmpty() || getMappedKeys().contains(key))
            map.insert(key, instance()->value(key));
    }

    if(result)
        *result=true;

    return map;
}

bool QSettingsMapper::submitSettings(const QStringList& dirtyKeys)
{
    QVariantMap dirtyMap;
    for(const QString& dirtyKey: dirtyKeys)
        dirtyMap.insert(dirtyKey, getStorage().value(dirtyKey));
    return toFile(dirtyMap);
}

bool QSettingsMapper::toFile(const QVariantMap& map)
{
    for(auto [key, value]: map.asKeyValueRange())
    {
        instance()->setValue(key, value);
    }

    return true;
}

QSettings *QSettingsMapper::instance()
{
    if (m_settings)
        return m_settings;

    if(!m_settingsPath.isEmpty() && !m_baseName.isEmpty())
    {
        const QString filePath = QDir(m_settingsPath).filePath(m_baseName);
        m_settings = new QSettings(filePath, QSettings::IniFormat, this);
    }
    else if(m_settingsPath.isEmpty() && !m_baseName.isEmpty())
    {
        const QString filePath = QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)).filePath(m_baseName);
        m_settings = new QSettings(filePath, QSettings::IniFormat, this);
    }
    else
    {
        m_settings = new QSettings(this);
    }

    if (m_settings->status() != QSettings::NoError)
    {
        // TODO: can't print out the enum due to the following error:
        // error: C2666: 'QQmlInfo::operator <<': 15 overloads have similar conversions
        QUTILSLOG_WARNING() << "Failed to initialize QSettings instance. Status code is: " << int(m_settings->status());
        if (m_settings->status() == QSettings::AccessError)
        {
            QStringList missingIdentifiers = {};
            if (QCoreApplication::organizationName().isEmpty())
                missingIdentifiers.append("organizationName");
            if (QCoreApplication::organizationDomain().isEmpty())
                missingIdentifiers.append("organizationDomain");
            if (QCoreApplication::applicationName().isEmpty())
                missingIdentifiers.append("applicationName");
            if (!missingIdentifiers.isEmpty()) {
                QUTILSLOG_WARNING() << "The following application identifiers have not been set: " << missingIdentifiers;
            }
        }
        return m_settings;
    }

    if (!m_settingsCategory.isEmpty())
        m_settings->beginGroup(m_settingsCategory);

    return m_settings;
}
