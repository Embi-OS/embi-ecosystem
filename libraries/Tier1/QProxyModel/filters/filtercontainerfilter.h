#ifndef FILTERCONTAINERFILTER_H
#define FILTERCONTAINERFILTER_H

#include "filter.h"
#include "filtercontainer.h"

namespace QQsfpm {

class FilterContainerFilter : public QQsfpm::Filter, public QQsfpm::FilterContainer {
    Q_OBJECT

    Q_INTERFACES(QQsfpm::FilterContainer)

    Q_PROPERTY(QQmlListProperty<QQsfpm::Filter> filters READ filtersListProperty NOTIFY filtersChanged FINAL)
    Q_CLASSINFO("DefaultProperty", "filters")

public:
    using QQsfpm::Filter::Filter;

    void proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel) override;

Q_SIGNALS:
    void filtersChanged();

private:
    void onFilterAppended(Filter* filter) override;
    void onFilterRemoved(Filter* filter) override;
    void onFiltersCleared() override;
};

}

#endif // FILTERCONTAINERFILTER_H
