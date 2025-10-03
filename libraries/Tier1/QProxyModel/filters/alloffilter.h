#ifndef ALLOFFILTER_H
#define ALLOFFILTER_H

#include "filtercontainerfilter.h"

namespace QQsfpm {

class AllOfFilter : public QQsfpm::FilterContainerFilter {
    Q_OBJECT

public:
    using QQsfpm::FilterContainerFilter::FilterContainerFilter;

protected:
    bool filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const override;
};

}

#endif // ALLOFFILTER_H
