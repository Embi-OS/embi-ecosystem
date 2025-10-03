#include "qstandardobjectgroup.h"
#include "axion_log.h"

QStandardObjectGroup::QStandardObjectGroup(QObject* parent):
    QObject(parent)
{

}

void QStandardObjectGroup::classBegin()
{

}

void QStandardObjectGroup::componentComplete()
{
    invalidate();
}

void QStandardObjectGroup::countInvalidate()
{
    const int aCount = count();
    bool emptyChanged=false;

    if(m_count==aCount)
        return;

    if((m_count==0 && aCount!=0) || (m_count!=0 && aCount==0))
        emptyChanged=true;

    m_count=aCount;
    emit this->countChanged(count());

    if(emptyChanged)
        emit this->emptyChanged(isEmpty());
}

void QStandardObjectGroup::queueInvalidate()
{
    if(!m_completed)
        return;

    if (m_delayed) {
        if (!m_invalidateQueued) {
            m_invalidateQueued = true;
            QMetaObject::invokeMethod(this, &QStandardObjectGroup::invalidate, Qt::QueuedConnection);
        }
    } else {
        invalidate();
    }
}

void QStandardObjectGroup::invalidate()
{
    m_completed = true;

    m_invalidateQueued = false;

    if(!m_enabled)
        return;

    emit aboutToBeInvalidated();

    int visibleCount=0;
    bool warning=false;
    bool isDirty=false;
    for(QStandardObject* object: m_objects)
    {
        if(!object->getVisible())
            continue;

        visibleCount++;

        if(m_enabledWarning)
            warning |= object->getWarning();

        if(m_enabledDirty)
            isDirty |= object->getIsDirty();
    }

    setVisibleCount(visibleCount);
    setWarning(warning);
    setIsDirty(isDirty);

    emit invalidated();
}

void QStandardObjectGroup::addObject(QObject* object)
{
    QStandardObject* standardObject = qobject_cast<QStandardObject*>(object);
    if(!standardObject || m_objects.contains(standardObject))
        return;
    referenceObject(standardObject);
    m_objects.append(standardObject);
    countInvalidate();
    queueInvalidate();
}

void QStandardObjectGroup::removeObject(QObject* object)
{
    QStandardObject* standardObject = qobject_cast<QStandardObject*>(object);
    if(!standardObject || m_objects.contains(standardObject))
        return;
    dereferenceObject(standardObject);
    const int index = m_objects.indexOf(standardObject);
    m_objects.remove(index);
    countInvalidate();
    queueInvalidate();
}

void QStandardObjectGroup::onObjectDestroyed(QObject* item)
{
    if(!item)
        return;
    const int index = m_objects.indexOf(item);
    m_objects.remove(index);
    countInvalidate();
    queueInvalidate();
}

void QStandardObjectGroup::onEnabledWarningChanged(bool enabledHeight)
{
    for(QStandardObject* item: m_objects)
    {
        dereferenceObject(item);
        referenceObject(item);
    }
    queueInvalidate();
}

void QStandardObjectGroup::onEnabledDirtyChanged(bool enabledWidth)
{
    for(QStandardObject* item: m_objects)
    {
        dereferenceObject(item);
        referenceObject(item);
    }
    queueInvalidate();
}

void QStandardObjectGroup::referenceObject(QStandardObject* item)
{
    connect(item, &QObject::destroyed, this, &QStandardObjectGroup::onObjectDestroyed, Qt::DirectConnection);
    connect(item, &QStandardObject::visibleChanged, this, &QStandardObjectGroup::queueInvalidate, Qt::UniqueConnection);

    if(m_enabledWarning)
        connect(item, &QStandardObject::warningChanged, this, &QStandardObjectGroup::queueInvalidate, Qt::UniqueConnection);

    if(m_enabledDirty)
        connect(item, &QStandardObject::isDirtyChanged, this, &QStandardObjectGroup::queueInvalidate, Qt::UniqueConnection);
}

void QStandardObjectGroup::dereferenceObject(QStandardObject* item)
{
    disconnect(this, nullptr, item, nullptr);
    disconnect(item, nullptr, this, nullptr);
}
