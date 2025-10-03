#ifndef SINGLEROLE_H
#define SINGLEROLE_H

#include "proxyrole.h"
#include <QPersistentModelIndex>

namespace QQsfpm {

class SingleRole : public QQsfpm::ProxyRole
{
    Q_OBJECT

    Q_WRITABLE_REF_PROPERTY(QString, name, Name, {})
    Q_WRITABLE_VAR_PROPERTY(bool, cached, Cached, false)

public:
    explicit SingleRole(QObject *parent = nullptr);

    QStringList names() override;

protected slots:
    void invalidate() override;

private:
    QVariant data(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel &proxyModel, const QString &name) final;
    virtual QVariant data(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel &proxyModel) = 0;

    QHash<QPersistentModelIndex, QVariant> m_cache;
};

}

#endif // SINGLEROLE_H
