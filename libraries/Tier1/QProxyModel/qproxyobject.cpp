#include "qproxyobject.h"
#include "qqmlsortfilterproxymodel.h"

QProxyObject::QProxyObject(QObject *parent) : QObject(parent)
{
    connect(this, &QProxyObject::enabledChanged, this, &QProxyObject::invalidated);
}

void QProxyObject::proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel)
{
    Q_UNUSED(proxyModel)
}

void QProxyObject::updateRoles(const QQmlSortFilterProxyModel& proxyModel)
{
    Q_UNUSED(proxyModel)
}

void QProxyObject::invalidate()
{
    if (m_enabled)
        emit this->invalidated();
}
