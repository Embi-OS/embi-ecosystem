#include "rolesorter.h"
#include "qqmlsortfilterproxymodel.h"
#include "qvariantcompare.h"

namespace QQsfpm {

/*!
    \qmltype RoleSorter
    \inherits Sorter
    \inqmlmodule SortFilterProxyModel
    \ingroup Sorters
    \brief Sorts rows based on a source model role.

    A RoleSorter is a simple \l Sorter that sorts rows based on a source model role.

    In the following example, rows with be sorted by their \c lastName role :
    \code
    SortFilterProxyModel {
       sourceModel: contactModel
       sorters: RoleSorter { roleName: "lastName" }
    }
    \endcode
*/

RoleSorter::RoleSorter(QObject *parent) : Sorter(parent)
{
    connect(this, &RoleSorter::roleNameChanged, this, &RoleSorter::roleNamesInvalidated);
    connect(this, &RoleSorter::roleChanged, this, &RoleSorter::invalidate);
    connect(this, &RoleSorter::caseSensitivityChanged, this, &RoleSorter::invalidate);
    connect(this, &RoleSorter::localeAwareChanged, this, &RoleSorter::invalidate);
}

void RoleSorter::updateRoles(const QQmlSortFilterProxyModel& proxyModel)
{
    int role = proxyModel.roleForName(m_roleName.toUtf8());
    setRole(role);

    Sorter::updateRoles(proxyModel);
}

QPair<QVariant, QVariant> RoleSorter::sourceData(const QModelIndex &sourceLeft, const QModelIndex& sourceRight, const QQmlSortFilterProxyModel& proxyModel) const
{
    QPair<QVariant, QVariant> pair;
    if (m_role == -1)
        return pair;

    pair.first.setValue(proxyModel.sourceData(sourceLeft, m_role));
    pair.second.setValue(proxyModel.sourceData(sourceRight, m_role));
    return pair;
}

int RoleSorter::compare(const QModelIndex &sourceLeft, const QModelIndex& sourceRight, const QQmlSortFilterProxyModel& proxyModel) const
{
    QPair<QVariant, QVariant> pair = sourceData(sourceLeft, sourceRight, proxyModel);
    QVariant leftValue = pair.first;
    QVariant rightValue = pair.second;

    return QQsfpm::compare(leftValue, rightValue, m_caseSensitivity, m_localeAware);
}

}
