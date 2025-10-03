#include "indexfilter.h"
#include "qqmlsortfilterproxymodel.h"
#include "qvariantcompare.h"

namespace QQsfpm {

/*!
    \qmltype IndexFilter
    \inherits Filter
    \inqmlmodule SortFilterProxyModel
    \ingroup Filters
    \brief Filters rows based on their source index.

    An IndexFilter is a filter allowing contents to be filtered based on their source model index.

    In the following example, only the first row of the source model will be accepted:
    \code
    SortFilterProxyModel {
       sourceModel: contactModel
       filters: IndexFilter {
           maximumIndex: 0
       }
    }
    \endcode
*/

IndexFilter::IndexFilter(QObject *parent) : Filter(parent)
{
    connect(this, &IndexFilter::minimumIndexChanged, this, &IndexFilter::invalidate);
    connect(this, &IndexFilter::maximumIndexChanged, this, &IndexFilter::invalidate);
    connect(this, &IndexFilter::indexesChanged, this, &IndexFilter::invalidate);
}

bool IndexFilter::filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const
{
    int sourceRowCount = proxyModel.sourceModel()->rowCount();
    int sourceRow = sourceIndex.row();

    bool minimumIsValid;
    int minimum = m_minimumIndex.toInt(&minimumIsValid);
    if (minimumIsValid) {
        int actualMinimum = minimum < 0 ? sourceRowCount + minimum : minimum;
        if (sourceRow < actualMinimum)
            return false;
    }

    bool maximumIsValid;
    int maximum = m_maximumIndex.toInt(&maximumIsValid);
    if (maximumIsValid) {
        int actualMaximum = maximum < 0 ? sourceRowCount + maximum : maximum;
        if (sourceRow > actualMaximum)
            return false;
    }

    if (!m_indexes.isEmpty()) {
        for (const QVariant &v: m_indexes) {
            bool isValid;
            int index = v.toInt(&isValid);
            if (isValid && sourceRow == index)
                return true;
        }
        return false;
    }

    return true;
}

}
