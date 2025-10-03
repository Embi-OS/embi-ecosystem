#include "rangefilter.h"
#include "qvariantcompare.h"

namespace QQsfpm {

/*!
    \qmltype RangeFilter
    \inherits RoleFilter
    \inqmlmodule SortFilterProxyModel
    \ingroup Filters
    \brief Filters rows between boundary values.

    A RangeFilter is a \l RoleFilter that accepts rows if their data is between the filter's minimum and maximum value.

    In the following example, only rows with their \c price role set to a value between the tow boundary of the slider will be accepted :
    \code
    RangeSlider {
       id: priceRangeSlider
    }

    SortFilterProxyModel {
       sourceModel: priceModel
       filters: RangeFilter {
           roleName: "price"
           minimumValue: priceRangeSlider.first.value
           maximumValue: priceRangeSlider.second.value
       }
    }
    \endcode
*/

RangeFilter::RangeFilter(QObject *parent) : RoleFilter(parent)
{
    connect(this, &RangeFilter::minimumValueChanged, this, &RangeFilter::invalidate);
    connect(this, &RangeFilter::minimumInclusiveChanged, this, &RangeFilter::invalidate);
    connect(this, &RangeFilter::maximumValueChanged, this, &RangeFilter::invalidate);
    connect(this, &RangeFilter::maximumInclusiveChanged, this, &RangeFilter::invalidate);
}

bool RangeFilter::filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const
{
    const QVariant value = sourceData(sourceIndex, proxyModel);
    bool lessThanMin = m_minimumValue.isValid() &&
            (m_minimumInclusive ? QQsfpm::lessThan(value, m_minimumValue, m_caseSensitivity, m_localeAware) : !QQsfpm::lessThan(m_minimumValue, value, m_caseSensitivity, m_localeAware));
    bool moreThanMax = m_maximumValue.isValid() &&
            (m_maximumInclusive ? QQsfpm::lessThan(m_maximumValue, value, m_caseSensitivity, m_localeAware) : !QQsfpm::lessThan(value, m_maximumValue, m_caseSensitivity, m_localeAware));
    return !(lessThanMin || moreThanMax);
}

}
