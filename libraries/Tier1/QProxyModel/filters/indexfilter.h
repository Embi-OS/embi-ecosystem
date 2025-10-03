#ifndef INDEXFILTER_H
#define INDEXFILTER_H

#include "filter.h"
#include <QVariant>

namespace QQsfpm {

class IndexFilter: public QQsfpm::Filter {
    Q_OBJECT

    Q_WRITABLE_REF_PROPERTY(QVariant, minimumIndex, MinimumIndex, {})
    Q_WRITABLE_REF_PROPERTY(QVariant, maximumIndex, MaximumIndex, {})
    Q_WRITABLE_REF_PROPERTY(QVariantList, indexes, Indexes, {})

public:
    explicit IndexFilter(QObject *parent = nullptr);

protected:
    bool filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const override;
};

}

#endif // INDEXFILTER_H
