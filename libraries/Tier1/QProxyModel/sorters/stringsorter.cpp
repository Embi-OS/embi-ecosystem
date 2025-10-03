#include "stringsorter.h"

namespace QQsfpm {

/*!
    \qmltype StringSorter
    \inherits RoleSorter
    \inqmlmodule SortFilterProxyModel
    \ingroup Sorters
    \brief Sorts rows based on a source model string role.

    \l StringSorter is a specialized \l RoleSorter that sorts rows based on a source model string role.
    \l StringSorter compares strings according to a localized collation algorithm.

    In the following example, rows with be sorted by their \c lastName role :
    \code
    SortFilterProxyModel {
       sourceModel: contactModel
       sorters: StringSorter { roleName: "lastName" }
    }
    \endcode
*/

StringSorter::StringSorter(QObject *parent) : RoleSorter(parent)
{
    connect(this, &StringSorter::caseSensitivityChanged, this, &StringSorter::updateCaseSensitivity);
    connect(this, &StringSorter::ignorePunctationChanged, this, &StringSorter::updateIgnorePunctation);
    connect(this, &StringSorter::localeChanged, this, &StringSorter::updateLocale);
    connect(this, &StringSorter::numericModeChanged, this, &StringSorter::updateNumericMode);
}

void StringSorter::updateCaseSensitivity()
{
    m_collator.setCaseSensitivity(m_caseSensitivity);
    invalidate();
}

void StringSorter::updateIgnorePunctation()
{
    m_collator.setIgnorePunctuation(m_ignorePunctation);
    invalidate();
}

void StringSorter::updateLocale()
{
    m_collator.setLocale(m_locale);
    invalidate();
}

void StringSorter::updateNumericMode()
{
    m_collator.setNumericMode(m_numericMode);
    invalidate();
}

int StringSorter::compare(const QModelIndex &sourceLeft, const QModelIndex &sourceRight, const QQmlSortFilterProxyModel& proxyModel) const
{
    QPair<QVariant, QVariant> pair = sourceData(sourceLeft, sourceRight, proxyModel);
    QString leftValue = pair.first.toString();
    QString rightValue = pair.second.toString();
    return m_collator.compare(leftValue, rightValue);
}

}
