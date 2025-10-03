#include "filter.h"
#include "qqmlsortfilterproxymodel.h"

namespace QQsfpm {

/*!
    \qmltype Filter
    \qmlabstract
    \inqmlmodule SortFilterProxyModel
    \ingroup Filters
    \brief Base type for the \l SortFilterProxyModel filters.

    The Filter type cannot be used directly in a QML file.
    It exists to provide a set of common properties and methods,
    available across all the other filter types that inherit from it.
    Attempting to use the Filter type directly will result in an error.
*/

Filter::Filter(QObject *parent) : QProxyObject(parent)
{
    connect(this, &Filter::invertedChanged, this, &Filter::invalidate);
}

bool Filter::filterAcceptsRow(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const
{
    return !m_enabled || filterRow(sourceIndex, proxyModel) ^ m_inverted;
}

}
