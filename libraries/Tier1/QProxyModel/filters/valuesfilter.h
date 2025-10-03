#ifndef VALUESFILTER_H
#define VALUESFILTER_H

#include "rolefilter.h"
#include <QVariant>

namespace QQsfpm {

class ValuesFilter : public QQsfpm::RoleFilter {
    Q_OBJECT

    Q_WRITABLE_REF_PROPERTY(QVariantList, values, Values, {})

public:
    explicit ValuesFilter(QObject *parent = nullptr);

protected:
    bool filterRow(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const override;
};

}

#endif // VALUESFILTER_H
