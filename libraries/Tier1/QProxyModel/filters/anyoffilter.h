#ifndef ANYOFFILTER_H
#define ANYOFFILTER_H

#include "filtercontainerfilter.h"

namespace QQsfpm {

class AnyOfFilter : public QQsfpm::FilterContainerFilter {
    Q_OBJECT

public:
    using QQsfpm::FilterContainerFilter::FilterContainerFilter;

protected:
    bool filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const override;
};

}

#endif // ANYOFFILTER_H
