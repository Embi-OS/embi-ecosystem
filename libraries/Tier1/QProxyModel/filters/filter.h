#ifndef FILTER_H
#define FILTER_H

#include "qproxyobject.h"

namespace QQsfpm {

class Filter : public QProxyObject
{
    Q_OBJECT

    Q_WRITABLE_VAR_PROPERTY(bool, inverted, Inverted, false)

public:
    explicit Filter(QObject *parent = nullptr);

    bool filterAcceptsRow(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const;

protected:
    virtual bool filterRow(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const = 0;
};

}

#endif // FILTER_H
