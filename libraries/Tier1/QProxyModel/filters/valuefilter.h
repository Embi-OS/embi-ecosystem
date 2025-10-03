#ifndef VALUEFILTER_H
#define VALUEFILTER_H

#include "rolefilter.h"
#include <QVariant>

namespace QQsfpm {

class ValueFilter : public QQsfpm::RoleFilter {
    Q_OBJECT

    Q_WRITABLE_REF_PROPERTY(QVariant, value, Value, {})

public:
    explicit ValueFilter(QObject *parent = nullptr);

protected:
    bool filterRow(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const override;
};

}

#endif // VALUEFILTER_H
