#ifndef RANGEFILTER_H
#define RANGEFILTER_H

#include "rolefilter.h"
#include <QVariant>

namespace QQsfpm {

class RangeFilter : public QQsfpm::RoleFilter
{
    Q_OBJECT

    Q_WRITABLE_REF_PROPERTY(QVariant, minimumValue, MinimumValue, {})
    Q_WRITABLE_VAR_PROPERTY(bool, minimumInclusive, MinimumInclusive, true)
    Q_WRITABLE_REF_PROPERTY(QVariant, maximumValue, MaximumValue, {})
    Q_WRITABLE_VAR_PROPERTY(bool, maximumInclusive, MaximumInclusive, true)
    Q_WRITABLE_VAR_PROPERTY(Qt::CaseSensitivity, caseSensitivity, CaseSensitivity, Qt::CaseSensitive)
    Q_WRITABLE_VAR_PROPERTY(bool, localeAware, LocaleAware, false)

public:
    explicit RangeFilter(QObject *parent = nullptr);

protected:
    bool filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const override;
};

}

#endif // RANGEFILTER_H
