#include "singlerole.h"
#include <QVariant>

namespace QQsfpm {

/*!
    \qmltype SingleRole
    \qmlabstract
    \inherits ProxyRole
    \inqmlmodule SortFilterProxyModel
    \ingroup ProxyRoles
    \brief Base type for the \l SortFilterProxyModel proxy roles defining a single role.

    SingleRole is a convenience base class for proxy roles who define a single role.
    It cannot be used directly in a QML file.
    It exists to provide a set of common properties and methods,
    available across all the other proxy role types that inherit from it.
    Attempting to use the SingleRole type directly will result in an error.
*/

SingleRole::SingleRole(QObject *parent) : ProxyRole(parent)
{
    connect(this, &SingleRole::nameChanged, this, &SingleRole::namesChanged);
}

QStringList SingleRole::names()
{
    return QStringList{ m_name };
}

void SingleRole::invalidate()
{
    m_cache.clear();
    ProxyRole::invalidate();
}

QVariant SingleRole::data(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel &proxyModel, const QString &name)
{
    Q_UNUSED(name);

    if (m_cached && m_cache.contains(sourceIndex)) {
        return m_cache.value(sourceIndex);
    }

    const QVariant result =  data(sourceIndex, proxyModel);

    if (m_cached) {
        m_cache[sourceIndex] = result;
    }

    return result;
}

}
