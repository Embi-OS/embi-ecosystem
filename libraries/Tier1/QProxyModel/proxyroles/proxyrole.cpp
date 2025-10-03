#include "proxyrole.h"
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlExpression>
#include <QDebug>
#include <QQmlInfo>
#include "filters/filter.h"
#include "qqmlsortfilterproxymodel.h"

namespace QQsfpm {

/*!
    \qmltype ProxyRole
    \inqmlmodule SortFilterProxyModel
    \ingroup ProxyRoles
    \brief Base type for the \l SortFilterProxyModel proxy roles.

    The ProxyRole type cannot be used directly in a QML file.
    It exists to provide a set of common properties and methods,
    available across all the other proxy role types that inherit from it.
    Attempting to use the ProxyRole type directly will result in an error.
*/

ProxyRole::ProxyRole(QObject *parent) : QProxyObject(parent)
{
    connect(this, &ProxyRole::defaultValueChanged, this, &ProxyRole::invalidate);
}

QVariant ProxyRole::roleData(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel, const QString &name)
{
    if(!m_enabled)
        return m_defaultValue;

    if (m_readMutex.tryLock()) {
        QVariant result = data(sourceIndex, proxyModel, name);
        m_readMutex.unlock();
        return result;
    } else {
        return {};
    }
}

bool ProxyRole::setRoleData(const QModelIndex& sourceIndex, const QVariant &value, const QQmlSortFilterProxyModel& proxyModel, const QString& name)
{
    if (m_writeMutex.tryLock()) {
        bool result = setData(sourceIndex, value, proxyModel, name);
        m_writeMutex.unlock();
        return result;
    } else {
        return false;
    }
}

void ProxyRole::markDirty(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);
    Q_UNUSED(roles);
}

bool ProxyRole::setData(const QModelIndex& sourceIndex, const QVariant &value, const QQmlSortFilterProxyModel& proxyModel, const QString& name)
{
    Q_UNUSED(sourceIndex);
    Q_UNUSED(value);
    Q_UNUSED(proxyModel);
    Q_UNUSED(name);
    return false;
}

}
