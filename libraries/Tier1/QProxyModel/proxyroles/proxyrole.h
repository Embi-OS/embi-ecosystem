#ifndef PROXYROLE_H
#define PROXYROLE_H

#include "qproxyobject.h"
#include <QMutex>

namespace QQsfpm {

class ProxyRole : public QProxyObject
{
    Q_OBJECT

    Q_WRITABLE_REF_PROPERTY(QVariant, defaultValue, DefaultValue, {})

public:
    explicit ProxyRole(QObject *parent = nullptr);

    QVariant roleData(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel, const QString& name);
    bool setRoleData(const QModelIndex& sourceIndex, const QVariant &value, const QQmlSortFilterProxyModel& proxyModel, const QString& name);

    virtual QStringList names() = 0;
    virtual void markDirty(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QList<int>());

signals:
    void namesChanged();

private:
    virtual QVariant data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel, const QString& name) = 0;
    virtual bool setData(const QModelIndex& sourceIndex, const QVariant &value, const QQmlSortFilterProxyModel& proxyModel, const QString& name);

    QMutex m_readMutex;
    QMutex m_writeMutex;
};

}

#endif // PROXYROLE_H
