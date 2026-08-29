#include "qsettingsmapper.h"
#include "qutils_log.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

Q_GLOBAL_STATIC(QString, g_path)
Q_GLOBAL_STATIC(QString, g_name)

namespace {

static QString settingsBaseName(const QString& baseName)
{
    QString normalizedBaseName = baseName;
    if (normalizedBaseName.isEmpty())
        normalizedBaseName = QCoreApplication::applicationName();

    if(!QFileInfo(baseName).suffix().isEmpty())
        return normalizedBaseName;

    return normalizedBaseName + QStringLiteral(".conf");
}

static QString settingsRootPath(const QString& settingsPath)
{
    if (!settingsPath.isEmpty())
        return settingsPath;

    return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
}

}

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

QSettings* QSettingsMapper::createSettings(QObject* parent)
{
    return createSettings(*g_path, *g_name, parent);
}

QSettings* QSettingsMapper::createSettings(const QString& baseName, QObject* parent)
{
    return createSettings(*g_path, baseName, parent);
}

QSettings* QSettingsMapper::createSettings(const QString& settingsPath, const QString& baseName, QObject* parent)
{
    QSettings* settings = nullptr;
#ifdef Q_OS_WASM
    const QString organizationName = QCoreApplication::organizationName();
    const QString applicationName = settingsBaseName(baseName);
    settings = new QSettings(QSettings::WebLocalStorageFormat,
                             QSettings::UserScope,
                             organizationName,
                             applicationName,
                             parent);
#else
    if(!baseName.isEmpty())
    {
        const QString rootPath = settingsRootPath(settingsPath);
        if(!rootPath.isEmpty() && !QDir().mkpath(rootPath)) {
            QUTILSLOG_WARNING() << "Failed to create settings path:" << rootPath;
        }

        const QString filePath = QDir(rootPath).filePath(settingsBaseName(baseName));
        settings = new QSettings(filePath, QSettings::IniFormat, parent);
    }
    else
    {
        settings = new QSettings(parent);
    }
#endif

    QUTILSLOG_DEBUG()<<"QSettings created at:"<<settings->fileName();

    if (settings->status() != QSettings::NoError)
    {
        // TODO: can't print out the enum due to the following error:
        // error: C2666: 'QQmlInfo::operator <<': 15 overloads have similar conversions
        QUTILSLOG_WARNING() << "Failed to initialize QSettings instance. Status code is: " << int(settings->status());
        if (settings->status() == QSettings::AccessError)
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
    }

    return settings;
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
#ifndef Q_OS_WASM
    const QString settingsPath = settingsRootPath(m_settingsPath);
    if (!m_baseName.isEmpty() && !settingsPath.isEmpty() && !QDir().mkpath(settingsPath)) {
        QUTILSLOG_WARNING() << "Failed to create settings path:" << settingsPath;
        return false;
    }
#endif

    for(auto [key, value]: map.asKeyValueRange())
    {
        instance()->setValue(key, value);
    }

    instance()->sync();
    return instance()->status() == QSettings::NoError;
}

QSettings *QSettingsMapper::instance()
{
    if (m_settings)
        return m_settings;

    m_settings = createSettings(m_settingsPath, m_baseName, this);

    if (m_settings->status()!=QSettings::NoError)
        return m_settings;

    if (!m_settingsCategory.isEmpty())
        m_settings->beginGroup(m_settingsCategory);

    return m_settings;
}
