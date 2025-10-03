#ifndef FILTERROLE_H
#define FILTERROLE_H

#include "singlerole.h"
#include "filters/filtercontainer.h"

namespace QQsfpm {

class FilterRole : public QQsfpm::SingleRole, public QQsfpm::FilterContainer
{
    Q_OBJECT
    Q_INTERFACES(QQsfpm::FilterContainer)

    Q_PROPERTY(QQmlListProperty<QQsfpm::Filter> filters READ filtersListProperty CONSTANT FINAL)
    Q_CLASSINFO("DefaultProperty", "filters")

public:
    using QQsfpm::SingleRole::SingleRole;

private:
    void onFilterAppended(Filter* filter) override;
    void onFilterRemoved(Filter* filter) override;
    void onFiltersCleared() override;

    QVariant data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) override;
};

}

#endif // FILTERROLE_H
