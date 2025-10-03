#include "valuesfilter.h"
#include <QDebug>

namespace QQsfpm {

/*!
    \qmltype ValuesFilter
    \inherits RoleFilter
    \inqmlmodule SortFilterProxyModel
    \ingroup Filters
    \brief Filters rows matching values.

    A ValuesFilter is a simple \l RoleFilter that accepts rows matching exactly the filter's values

    In the following example, only rows with their \c favorite role set to \c true will be accepted when the checkbox is checked :
    \code
    CheckBox {
       id: showOnlyFavoriteCheckBox
    }

    SortFilterProxyModel {
       sourceModel: contactModel
       filters: ValuesFilter {
           roleName: "favorite"
           values: ["yes", "maybe"]
           enabled: showOnlyFavoriteCheckBox.checked
       }
    }
    \endcode

*/

ValuesFilter::ValuesFilter(QObject *parent) : RoleFilter(parent)
{
    connect(this, &ValuesFilter::valuesChanged, this, &ValuesFilter::invalidate);
}

bool ValuesFilter::filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const
{
    const QVariant srcVal = sourceData(sourceIndex, proxyModel);

    if(srcVal.metaType().id() == QMetaType::QVariantList)
    {
        const QVariantList& variants = *reinterpret_cast<const QVariantList*>(srcVal.constData());
        return std::any_of(variants.begin(), variants.end(), [&] (const QVariant& value) {
            return m_values.contains(value);
        });
    }
    else
    {
        return m_values.contains(srcVal);
    }
}

}
