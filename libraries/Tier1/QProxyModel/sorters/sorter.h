#ifndef SORTER_H
#define SORTER_H

#include "qproxyobject.h"

namespace QQsfpm {

class Sorter : public QProxyObject
{
    Q_OBJECT

    Q_WRITABLE_VAR_PROPERTY(bool, ascendingOrder, AscendingOrder, true)
    Q_WRITABLE_VAR_PROPERTY(Qt::SortOrder, sortOrder, SortOrder, Qt::AscendingOrder)
    Q_WRITABLE_VAR_PROPERTY(int, priority, Priority, 0)

public:
    explicit Sorter(QObject* parent = nullptr);

    int compareRows(const QModelIndex& source_left, const QModelIndex& source_right, const QQmlSortFilterProxyModel& proxyModel) const;

protected:
    virtual int compare(const QModelIndex& sourceLeft, const QModelIndex& sourceRight, const QQmlSortFilterProxyModel& proxyModel) const;
    virtual bool lessThan(const QModelIndex& sourceLeft, const QModelIndex& sourceRight, const QQmlSortFilterProxyModel& proxyModel) const;

private slots:
    void updateAscendingOrder();
};

}

#endif // SORTER_H
