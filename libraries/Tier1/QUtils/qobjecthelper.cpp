#include "qobjecthelper.h"
#include "qutils_log.h"

QObjectHelper::QObjectHelper(QObject* object) :
    QObject(object),
    m_object(object)
{

}

QObjectHelper* QObjectHelper::wrap(const QObject* object)
{
    return qobject_cast<QObjectHelper*>(qmlAttachedPropertiesObject<QObjectHelper>(object, true));
}

QObjectHelper* QObjectHelper::qmlAttachedProperties(QObject* object)
{
    if (!object)
    {
        QUTILSLOG_CRITICAL()<<object;
        QUTILSLOG_FATAL("QObjectHelper must be attached to a QObject*");
        return nullptr;
    }

    QObjectHelper* helper = new QObjectHelper(object);
    QQmlEngine::setObjectOwnership(helper, QQmlEngine::CppOwnership);

    return helper;

}

// ──────── PUBLIC API ──────────
bool QObjectHelper::set(const QVariantMap& data)
{
    return set(m_object, data);
}

bool QObjectHelper::setProperty(const QString& property, const QVariant& value)
{
    return setProperty(m_object, property, value);
}

QVariantMap QObjectHelper::get(const QStringList& properties) const
{
    return get(m_object, properties);
}

QVariant QObjectHelper::getProperty(const QString& property) const
{
    return getProperty(m_object, property);
}

bool QObjectHelper::reset(const QStringList& properties)
{
    return reset(m_object, properties);
}

bool QObjectHelper::resetProperty(const QString& property)
{
    return resetProperty(m_object, property);
}

// ──────── PUBLIC STATIC API ──────────
bool QObjectHelper::set(QObject* object, const QVariantMap& data)
{
    if(!object)
        return false;

    bool result = true;

    const QMetaObject* mo = object->metaObject();
    for(auto [key, value]: data.asKeyValueRange())
    {
        const int index = mo->indexOfProperty(key.toUtf8());
        if(index<0)
            continue;
        const QMetaProperty mp = mo->property(index);
        if(!writeProperty(object, mp, value))
            result = false;
    }

    return result;
}

bool QObjectHelper::setProperty(QObject* object, const QString& property, const QVariant& value)
{
    if(!object)
        return false;

    const QMetaObject* mo = object->metaObject();
    const int index = mo->indexOfProperty(property.toUtf8());
    if(index<0)
        return false;

    const QMetaProperty mp = mo->property(index);
    bool result = writeProperty(object, mp, value);

    return result;
}

QVariantMap QObjectHelper::get(const QObject* object, const QStringList& properties)
{
    if(!object)
        return QVariantMap();

    QVariantMap data;

    const QMetaObject* mo = object->metaObject();
    if(properties.isEmpty())
    {
        const int count = mo->propertyCount();
        for(int i=0; i<count; i++)
        {
            QMetaProperty mp = mo->property(i);
            const QString &propertyName = mp.name();
            data.insert(propertyName, readProperty(object, mp));
        }
    }
    else
    {
        for(const QString& property: properties)
        {
            const int index = mo->indexOfProperty(property.toUtf8());
            if(index<0)
                continue;
            const QMetaProperty mp = mo->property(index);
            data.insert(property, readProperty(object, mp));
        }
    }

    return data;
}

QVariant QObjectHelper::getProperty(const QObject* object, const QString& property)
{
    if(!object)
        return QVariant();

    const QMetaObject* mo = object->metaObject();
    const int index = mo->indexOfProperty(property.toUtf8());
    if(index<0)
        return QVariant();

    const QMetaProperty mp = mo->property(index);
    const QVariant value = readProperty(object, mp);

    return value;
}

bool QObjectHelper::reset(QObject* object, const QStringList& properties)
{
    if(!object)
        return false;

    bool result = true;
    const QMetaObject* mo = object->metaObject();
    if(properties.isEmpty())
    {
        const int count = mo->propertyCount();
        for(int i=0; i<count; i++)
        {
            QMetaProperty mp = mo->property(i);
            if(!clearProperty(object, mp))
                result = false;
        }
    }
    else
    {
        for(const QString& property: properties)
        {
            const int index = mo->indexOfProperty(property.toUtf8());
            if(index<0)
                continue;
            const QMetaProperty mp = mo->property(index);
            if(!clearProperty(object, mp))
                result = false;
        }
    }

    return result;
}

bool QObjectHelper::resetProperty(QObject* object, const QString& property)
{
    if(!object)
        return false;

    const QMetaObject* mo = object->metaObject();
    const int index = mo->indexOfProperty(property.toUtf8());
    if(index<0)
        return false;

    const QMetaProperty mp = mo->property(index);
    bool result = clearProperty(object, mp);

    return result;
}

QVariant QObjectHelper::readProperty(const QObject* object, const QMetaProperty &property)
{
    QVariant value = property.read(object);
    if (property.isEnumType())
        value = value.toInt();
    else if (value.userType() == qMetaTypeId<QJSValue>())
        value = value.value<QJSValue>().toVariant();
    return value;
}

bool QObjectHelper::writeProperty(QObject *object, const QMetaProperty &property, const QVariant &value)
{
    if(property.write(object, value))
        return true;
    return property.write(object, QVariant(property.metaType()));
}

bool QObjectHelper::clearProperty(QObject *object, const QMetaProperty &property)
{
    return property.reset(object);
}
