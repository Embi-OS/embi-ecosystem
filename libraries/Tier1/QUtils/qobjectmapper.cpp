#include "qobjectmapper.h"
#include "qutils_log.h"

QObjectMapper::QObjectMapper(QObject *parent) :
    QObjectMapper("", parent)
{

}

QObjectMapper::QObjectMapper(const QString& baseName, QObject *parent) :
    QVariantMapper(baseName, parent, &QObjectMapper::staticMetaObject)
{
    connect(this, &QObjectMapper::objectAboutToChange, this, &QObjectMapper::onObjectAboutToChange);
    connect(this, &QObjectMapper::objectChanged, this, &QObjectMapper::onObjectChanged);
}

QObjectMapper::~QObjectMapper()
{
    if(m_submitPolicy<QVariantMapperPolicies::Manual && m_isDirty) {
        submit();
        waitForSubmit();
    }
}

bool QObjectMapper::doSelect()
{
    bool result = true;
    const QVariantMap storage = selectObject(&result);
    setStorage(storage);
    emitSelectDone(result);

    return true;
}

bool QObjectMapper::doSubmit(const QStringList& dirtyKeys)
{
    bool result = submitObject(dirtyKeys);
    emitSubmitDone(result);

    return result;
}

void QObjectMapper::onObjectAboutToChange(QObject* oldObject, QObject* newObject)
{
    if(oldObject!=nullptr) {
        disconnect(oldObject, nullptr,this, nullptr);
    }
}

void QObjectMapper::onObjectChanged(QObject* object)
{
    if (object != nullptr) {
        const QMetaObject *mo = object->metaObject();
        const int count = mo->propertyCount();

        for(int i=0; i<count; i++)
        {
            QMetaProperty mp = mo->property(i);
            if (mp.hasNotifySignal())
            {
                static const char* HANDLER = "onObjectPropertyChanged()";
                const QMetaMethod notifySignal = mp.notifySignal();
                const QMetaMethod updateSlot = metaObject()->method(metaObject()->indexOfSlot(HANDLER));
                connect(object,notifySignal,this,updateSlot);
            }
        }
    }

    queueSelect();
}

void QObjectMapper::onObjectPropertyChanged()
{
    if(m_loading)
        return;

    QObject *object = sender();
    if(object!=m_object)
        return;

    queueSelect();
}

QVariantMap QObjectMapper::selectObject(bool* result)
{
    if(!m_object)
    {
        if(result)
            *result=false;
        return QVariantMap();
    }

    return fromObject();
}

QVariantMap QObjectMapper::fromObject(bool* result)
{
    if(!m_object)
    {
        QUTILSLOG_WARNING()<<"No model given";
        if(result)
            *result=false;
        return QVariantMap();
    }

    const QMetaObject *mo = m_object->metaObject();
    const int count = mo->propertyCount();

    QVariantMap map;
    for(int i=0; i<count; i++)
    {
        QMetaProperty mp = mo->property(i);
        const QString &propertyName = mp.name();
        if(getMappedKeys().isEmpty() || getMappedKeys().contains(propertyName))
            map.insert(propertyName, mp.read(m_object));
    }

    if(result)
        *result=true;

    return map;
}

bool QObjectMapper::submitObject(const QStringList& dirtyKeys)
{
    if(!m_object)
        return false;

    QVariantMap dirtyMap;
    for(const QString& dirtyKey: dirtyKeys)
        dirtyMap.insert(dirtyKey, getStorage().value(dirtyKey));
    return toObject(dirtyMap);
}

bool QObjectMapper::toObject(const QVariantMap& map) const
{
    if(!m_object)
    {
        QUTILSLOG_WARNING()<<"No model given";
        return false;
    }

    const QMetaObject *mo = m_object->metaObject();

    for(auto [key, value]: map.asKeyValueRange())
    {
        const int index = mo->indexOfProperty(key.toUtf8());
        if (index<0)
            continue;

        const QMetaProperty mp = mo->property(index);
        if(!mp.write(m_object, value))
            mp.write(m_object, QVariant(mp.metaType()));
    }

    return true;
}
