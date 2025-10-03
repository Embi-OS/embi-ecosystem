#include "qqmlsortfilterproxymodel.h"
#include <algorithm>
#include "filters/filter.h"
#include "sorters/sorter.h"
#include "proxyroles/proxyrole.h"

QQmlSortFilterProxyModel::QQmlSortFilterProxyModel(QObject *parent) :
    QProxyModel(parent)
{

}

QVariant QQmlSortFilterProxyModel::sourceData(const QModelIndex& sourceIndex, const QString& roleName) const
{
    return QProxyModel::sourceData(sourceIndex, roleName);
}

QVariant QQmlSortFilterProxyModel::sourceData(const QModelIndex &sourceIndex, int role) const
{
    if(role<=m_proxyRolesOffset)
        return QProxyModel::sourceData(sourceIndex, role);

    QPair<QQsfpm::ProxyRole*, QString> proxyRolePair = m_proxyRoleMap[role];
    if (QQsfpm::ProxyRole* proxyRole = proxyRolePair.first)
        return proxyRole->roleData(sourceIndex, *this, proxyRolePair.second);

    return QVariant();
}

bool QQmlSortFilterProxyModel::setSourceData(const QModelIndex& sourceIndex, const QVariant &value, const QString& roleName) const
{
    return QProxyModel::setSourceData(sourceIndex, value, roleName);
}

bool QQmlSortFilterProxyModel::setSourceData(const QModelIndex& sourceIndex, const QVariant &value, int role) const
{
    if(role<=m_proxyRolesOffset)
        return QProxyModel::setSourceData(sourceIndex, value, role);

    QPair<QQsfpm::ProxyRole*, QString> proxyRolePair = m_proxyRoleMap[role];
    if (QQsfpm::ProxyRole* proxyRole = proxyRolePair.first)
        return proxyRole->setRoleData(sourceIndex, value, *this, proxyRolePair.second);

    return false;
}

void QQmlSortFilterProxyModel::invalidate()
{
    if(!m_isActive)
    {
        for (const auto& filter : std::as_const(m_filters))
            filter->proxyModelCompleted(*this);
        for (const auto& sorter : std::as_const(m_sorters))
            sorter->proxyModelCompleted(*this);
        for (const auto& proxyRole : std::as_const(m_proxyRoles))
            proxyRole->proxyModelCompleted(*this);
    }

    m_sortedSorters = m_sorters;
    std::stable_sort(m_sortedSorters.begin(), m_sortedSorters.end(), [] (QQsfpm::Sorter* a, QQsfpm::Sorter* b) {
        return a->getPriority() > b->getPriority();
    });

    QProxyModel::invalidate();
}

bool QQmlSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    if (!m_isActive || !m_enabled || !sourceModel())
        return false;

    if(!m_filterEnabled)
        return true;

    bool baseAcceptsRow = QProxyModel::filterAcceptsRow(source_row, source_parent) ^ getFilterInverted();

    if(baseAcceptsRow)
    {
        QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);
        baseAcceptsRow = baseAcceptsRow && std::all_of(m_filters.begin(), m_filters.end(),
            [this, sourceIndex] (QQsfpm::Filter* filter) {
                return filter->filterAcceptsRow(sourceIndex, *this) ^ getFilterInverted();
            }
        );
    }

    return baseAcceptsRow ^ getFilterInverted();
}

bool QQmlSortFilterProxyModel::lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const
{
    if (m_isActive && m_enabled && m_sortEnabled)
    {
        int comparison = compare(source_left, source_right);
        if (comparison != 0)
            return comparison < 0;

        for(auto sorter : m_sortedSorters) {
            if (sorter->getEnabled()) {
                int sorterComparison = sorter->compareRows(source_left, source_right, *this);
                if (sorterComparison != 0)
                    return sorterComparison < 0;
            }
        }
    }
    return source_left.row() < source_right.row();
}

void QQmlSortFilterProxyModel::updateRoleNames()
{
    if (!sourceModel() || (sourceModel()->roleNames().isEmpty()))
        return;
    m_roleNames = sourceModel()->roleNames();
    m_proxyRoleMap.clear();
    m_proxyRoleNumbers.clear();

    auto roles = m_roleNames.keys();
    auto maxIt = std::max_element(roles.cbegin(), roles.cend());
    int maxRole = maxIt != roles.cend() ? *maxIt : -1;
    m_proxyRolesOffset = maxRole;
    for (auto proxyRole : std::as_const(m_proxyRoles)) {
        const QStringList proxyRoleNames = proxyRole->names();
        for (const QString& roleName : proxyRoleNames) {
            ++maxRole;
            m_roleNames[maxRole] = roleName.toUtf8();
            m_proxyRoleMap[maxRole] = {proxyRole, roleName};
            m_proxyRoleNumbers.append(maxRole);
        }
    }

    reverseStringIntHash(m_roleIds, m_roleNames);

    emit this->roleNamesChanged();
}

void QQmlSortFilterProxyModel::updateRoles()
{
    QProxyModel::updateRoles();

    for (const auto& filter : std::as_const(m_filters))
        filter->updateRoles(*this);
    for (const auto& sorter : std::as_const(m_sorters))
        sorter->updateRoles(*this);
    for (const auto& proxyRole : std::as_const(m_proxyRoles))
        proxyRole->updateRoles(*this);
}

void QQmlSortFilterProxyModel::updateObjectRoles()
{
    QProxyObject* object = qobject_cast<QProxyObject*>(sender());
    if(object)
        object->updateRoles(*this);
}

void QQmlSortFilterProxyModel::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    QProxyModel::onDataChanged(topLeft, bottomRight, roles);

    if (!roles.isEmpty() && !m_proxyRoleNumbers.empty() && roles != m_proxyRoleNumbers) {
        emit this->dataChanged(topLeft, bottomRight, m_proxyRoleNumbers);
    }
}

void QQmlSortFilterProxyModel::queueInvalidateProxyRoles()
{
    queueInvalidate();
    if (m_delayed) {
        if (!m_invalidateProxyRolesQueued) {
            m_invalidateProxyRolesQueued = true;
            QMetaObject::invokeMethod(this, &QQmlSortFilterProxyModel::invalidateProxyRoles, Qt::QueuedConnection);
        }
    } else {
        invalidateProxyRoles();
    }
}

void QQmlSortFilterProxyModel::invalidateProxyRoles()
{
    m_invalidateProxyRolesQueued = false;
    if (m_isActive)
        emit this->dataChanged(index(0,0), index(rowCount() - 1, columnCount() - 1), m_proxyRoleNumbers);
}

void QQmlSortFilterProxyModel::onFilterAppended(QQsfpm::Filter* filter)
{
    if(m_isActive)
        filter->proxyModelCompleted(*this);

    connect(filter, &QQsfpm::Filter::invalidated, this, &QQmlSortFilterProxyModel::queueInvalidateFilter);
    connect(filter, &QQsfpm::Filter::roleNamesInvalidated, this, &QQmlSortFilterProxyModel::updateObjectRoles);
    queueInvalidateFilter();
}

void QQmlSortFilterProxyModel::onFilterRemoved(QQsfpm::Filter* filter)
{
    Q_UNUSED(filter)
    queueInvalidateFilter();
}

void QQmlSortFilterProxyModel::onFiltersCleared()
{
    queueInvalidateFilter();
}

void QQmlSortFilterProxyModel::onSorterAppended(QQsfpm::Sorter* sorter)
{
    if(m_isActive)
        sorter->proxyModelCompleted(*this);

    connect(sorter, &QQsfpm::Sorter::invalidated, this, &QQmlSortFilterProxyModel::queueInvalidate);
    connect(sorter, &QQsfpm::Sorter::roleNamesInvalidated, this, &QQmlSortFilterProxyModel::updateObjectRoles);
    queueInvalidate();
}

void QQmlSortFilterProxyModel::onSorterRemoved(QQsfpm::Sorter* sorter)
{
    Q_UNUSED(sorter)
    queueInvalidate();
}

void QQmlSortFilterProxyModel::onSortersCleared()
{
    queueInvalidate();
}

void QQmlSortFilterProxyModel::onProxyRoleAppended(QQsfpm::ProxyRole *proxyRole)
{
    if(m_isActive)
        proxyRole->proxyModelCompleted(*this);

    beginResetModel();
    connect(proxyRole, &QQsfpm::ProxyRole::roleNamesInvalidated, this, &QQmlSortFilterProxyModel::updateObjectRoles);
    connect(proxyRole, &QQsfpm::ProxyRole::invalidated, this, &QQmlSortFilterProxyModel::queueInvalidateProxyRoles);
    connect(proxyRole, &QQsfpm::ProxyRole::namesChanged, this, &QQmlSortFilterProxyModel::queueInvalidate);
    endResetModel();
}

void QQmlSortFilterProxyModel::onProxyRoleRemoved(QQsfpm::ProxyRole *proxyRole)
{
    Q_UNUSED(proxyRole)
    beginResetModel();
    endResetModel();
}

void QQmlSortFilterProxyModel::onProxyRolesCleared()
{
    beginResetModel();
    endResetModel();
}
