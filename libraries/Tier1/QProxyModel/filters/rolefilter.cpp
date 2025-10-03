#include "rolefilter.h"
#include "qqmlsortfilterproxymodel.h"

namespace QQsfpm {

/*!
    \qmltype RoleFilter
    \qmlabstract
    \inherits Filter
    \inqmlmodule SortFilterProxyModel
    \ingroup Filters
    \brief Base type for filters based on a source model role.

    The RoleFilter type cannot be used directly in a QML file.
    It exists to provide a set of common properties and methods,
    available across all the other filter types that inherit from it.
    Attempting to use the RoleFilter type directly will result in an error.
*/

RoleFilter::RoleFilter(QObject *parent) : Filter(parent)
{
    connect(this, &RoleFilter::roleNameChanged, this, &RoleFilter::roleNamesInvalidated);
    connect(this, &RoleFilter::roleChanged, this, &RoleFilter::invalidate);
}

void RoleFilter::updateRoles(const QQmlSortFilterProxyModel& proxyModel)
{
    int role = proxyModel.roleForName(m_roleName);
    setRole(role);

    Filter::updateRoles(proxyModel);
}

QVariant RoleFilter::sourceData(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const
{
    return proxyModel.sourceData(sourceIndex, m_role);
}

}
