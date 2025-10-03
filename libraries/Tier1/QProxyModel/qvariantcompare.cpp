#include "qvariantcompare.h"

#include <QDebug>
#include <QDateTime>
#include <QAbstractItemModel>

namespace QQsfpm {

/*!
    \brief Compare operator for generic QVariants
    Since Qt 5.15 deprecated the less-than operator of QVariant, we
    have to provide our own implementation. On older Qt versions,
    use the original implementation.
    Includes special implementations for numberic types, char, date and
    time. Everything else is converted to String and compared then.
*/
int ordering(const QVariant &left, const QVariant &right)
{
    return ::qVariantOrdering(left, right);
}

int compare(const QVariant &left, const QVariant &right, Qt::CaseSensitivity cs, bool isLocaleAware)
{
    return ::qVariantCompare(left, right, cs, isLocaleAware);
}

bool lessThan(const QVariant &left, const QVariant &right, Qt::CaseSensitivity cs, bool isLocaleAware)
{
    return ::qVariantLessThan(left, right, cs, isLocaleAware);
}

int indexOf(const QAbstractItemModel* model, int role, const QVariant& val)
{
    int index = -1;
    int left = 0;
    int right = (int (model->rowCount()) -1);

    while (left <= right)
    {
        const int middle = (left + (right - left) / 2);
        const QVariant& var = model->data(model->index(middle,0), role);
        const QPartialOrdering ordering = QVariant::compare(var, val);
        if (ordering == QPartialOrdering::Equivalent)
        {
            index = middle;
            break;
        }
        else if (ordering == QPartialOrdering::Less)
        {
            left = middle + 1;
        }
        else
        {
            right = middle - 1;
        }
    }

    return index;
}

} // namespace QQsfpm
