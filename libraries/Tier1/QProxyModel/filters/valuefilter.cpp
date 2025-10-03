#include "valuefilter.h"

namespace QQsfpm {

/*!
    \qmltype ValueFilter
    \inherits RoleFilter
    \inqmlmodule SortFilterProxyModel
    \ingroup Filters
    \brief Filters rows matching exactly a value.

    A ValueFilter is a simple \l RoleFilter that accepts rows matching exactly the filter's value

    In the following example, only rows with their \c favorite role set to \c true will be accepted when the checkbox is checked :
    \code
    CheckBox {
       id: showOnlyFavoriteCheckBox
    }

    SortFilterProxyModel {
       sourceModel: contactModel
       filters: ValueFilter {
           roleName: "favorite"
           value: true
           enabled: showOnlyFavoriteCheckBox.checked
       }
    }
    \endcode

*/

ValueFilter::ValueFilter(QObject *parent) : RoleFilter(parent)
{
    connect(this, &ValueFilter::valueChanged, this, &ValueFilter::invalidate);
}

bool ValueFilter::filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const
{
    const QVariant srcVal = sourceData(sourceIndex, proxyModel);

    return !m_value.isValid() || (m_value==srcVal);
}

}
