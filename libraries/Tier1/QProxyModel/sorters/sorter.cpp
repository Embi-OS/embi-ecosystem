#include "sorter.h"
#include "qqmlsortfilterproxymodel.h"

namespace QQsfpm {

/*!
    \qmltype Sorter
    \qmlabstract
    \inqmlmodule SortFilterProxyModel
    \ingroup Sorters
    \brief Base type for the \l SortFilterProxyModel sorters.

    The Sorter type cannot be used directly in a QML file.
    It exists to provide a set of common properties and methods,
    available across all the other sorters types that inherit from it.
    Attempting to use the Sorter type directly will result in an error.
*/

Sorter::Sorter(QObject *parent) : QProxyObject(parent)
{
    connect(this, &Sorter::ascendingOrderChanged, this, &Sorter::updateAscendingOrder);

    connect(this, &Sorter::sortOrderChanged, this, &Sorter::invalidate);
    connect(this, &Sorter::priorityChanged, this, &Sorter::invalidate);
}

int Sorter::compareRows(const QModelIndex &source_left, const QModelIndex &source_right, const QQmlSortFilterProxyModel& proxyModel) const
{
    int comparison = compare(source_left, source_right, proxyModel);
    return (m_sortOrder == Qt::AscendingOrder) ? comparison : -comparison;
}

int Sorter::compare(const QModelIndex &sourceLeft, const QModelIndex &sourceRight, const QQmlSortFilterProxyModel& proxyModel) const
{
    if (lessThan(sourceLeft, sourceRight, proxyModel))
        return -1;
    if (lessThan(sourceRight, sourceLeft, proxyModel))
        return 1;
    return 0;
}

bool Sorter::lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight, const QQmlSortFilterProxyModel& proxyModel) const
{
    Q_UNUSED(sourceLeft)
    Q_UNUSED(sourceRight)
    Q_UNUSED(proxyModel)
    return false;
}

void Sorter::updateAscendingOrder()
{
    setSortOrder(m_ascendingOrder ? Qt::AscendingOrder : Qt::DescendingOrder);
}

}
