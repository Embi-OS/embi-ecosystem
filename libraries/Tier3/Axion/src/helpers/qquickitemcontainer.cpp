#include "qquickitemcontainer.h"
#include "axion_log.h"

QQuickItemContainer::QQuickItemContainer(QObject* parent):
    QObject(parent)
{

}

void QQuickItemContainer::classBegin()
{

}

void QQuickItemContainer::componentComplete()
{
    invalidate();
}

void QQuickItemContainer::countInvalidate()
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

void QQuickItemContainer::queueInvalidate()
{
    if(!m_completed)
        return;

    if (m_delayed) {
        if (!m_invalidateQueued) {
            m_invalidateQueued = true;
            QMetaObject::invokeMethod(this, &QQuickItemContainer::invalidate, Qt::QueuedConnection);
        }
    } else {
        invalidate();
    }
}

void QQuickItemContainer::invalidate()
{
    m_completed = true;

    m_invalidateQueued = false;

    if(!m_enabled)
        return;

    emit aboutToBeInvalidated();

    double maxImplicitHeight=0;
    double totalImplicitHeight=0;
    double maxImplicitWidth=0;
    double totalImplicitWidth=0;
    int visibleCount=0;
    for(QQuickItem* item: std::as_const(m_items))
    {
        if(!item->isVisible())
            continue;

        visibleCount++;

        if(m_enabledHeight) {
            if(item->implicitHeight()>maxImplicitHeight)
                maxImplicitHeight = item->implicitHeight();
            totalImplicitHeight += item->implicitHeight();
        }

        if(m_enabledWidth) {
            if(item->implicitWidth()>maxImplicitWidth)
                maxImplicitWidth = item->implicitWidth();
            totalImplicitWidth += item->implicitWidth();
        }
    }

    setVisibleCount(visibleCount);
    setMaxImplicitHeight(maxImplicitHeight);
    setTotalImplicitHeight(totalImplicitHeight);
    setMaxImplicitWidth(maxImplicitWidth);
    setTotalImplicitWidth(totalImplicitWidth);

    emit invalidated();
}

void QQuickItemContainer::addItem(QQuickItem* item)
{
    if(!item || m_items.contains(item))
        return;
    referenceObject(item);
    m_items.append(item);
    countInvalidate();
    queueInvalidate();

    emit this->itemAdded(item);
}

void QQuickItemContainer::removeItem(QQuickItem* item)
{
    if(!item || !m_items.contains(item))
        return;
    dereferenceObject(item);
    const int index = m_items.indexOf(item);
    m_items.remove(index);
    countInvalidate();
    queueInvalidate();

    emit this->itemRemoved(item);
}

void QQuickItemContainer::onObjectDestroyed(QObject* item)
{
    if(!item)
        return;
    const int index = m_items.indexOf(item);
    m_items.remove(index);
    countInvalidate();
    queueInvalidate();
}

void QQuickItemContainer::onEnabledHeightChanged(bool enabledHeight)
{
    for(QQuickItem* item: std::as_const(m_items))
    {
        dereferenceObject(item);
        referenceObject(item);
    }
    queueInvalidate();
}

void QQuickItemContainer::onEnabledWidthChanged(bool enabledWidth)
{
    for(QQuickItem* item: std::as_const(m_items))
    {
        dereferenceObject(item);
        referenceObject(item);
    }
    queueInvalidate();
}

void QQuickItemContainer::referenceObject(QQuickItem* item)
{
    connect(item, &QObject::destroyed, this, &QQuickItemContainer::onObjectDestroyed, Qt::DirectConnection);
    connect(item, &QQuickItem::visibleChanged, this, &QQuickItemContainer::queueInvalidate, Qt::UniqueConnection);

    if(m_enabledHeight)
        connect(item, &QQuickItem::implicitHeightChanged, this, &QQuickItemContainer::queueInvalidate, Qt::UniqueConnection);

    if(m_enabledWidth)
        connect(item, &QQuickItem::implicitWidthChanged, this, &QQuickItemContainer::queueInvalidate, Qt::UniqueConnection);
}

void QQuickItemContainer::dereferenceObject(QQuickItem* item)
{
    disconnect(this, nullptr, item, nullptr);
    disconnect(item, nullptr, this, nullptr);
}

QQuickItemContainerAttached* QQuickItemContainer::qmlAttachedProperties(QObject* object)
{
    QQuickItem* item = qobject_cast<QQuickItem*>(object);
    if (!item)
    {
        AXIONLOG_FATAL("QQuickItemContainerAttached must be attached to a QQuickItem*");
        return nullptr;
    }

    QQuickItemContainerAttached* helper = new QQuickItemContainerAttached(object);
    QQmlEngine::setObjectOwnership(helper, QQmlEngine::CppOwnership);

    return helper;
}

QQuickItemContainerAttached::QQuickItemContainerAttached(QObject *parent):
    QObject(parent),
    m_item(qobject_cast<QQuickItem*>(parent)),
    m_container(nullptr)
{

}

QQuickItemContainerAttached* QQuickItemContainerAttached::wrap(const QObject* object)
{
    return qobject_cast<QQuickItemContainerAttached*>(qmlAttachedPropertiesObject<QQuickItemContainer>(object, true));
}

QQuickItemContainer* QQuickItemContainerAttached::getContainer() const
{
    return m_container;
}

void QQuickItemContainerAttached::setContainer(QQuickItemContainer* container)
{
    if(m_container==container)
        return;

    if(m_container)
        m_container->removeItem(m_item);

    m_container = container;

    if(m_container)
        m_container->addItem(m_item);

    emit this->containerChanged(m_container);
}
