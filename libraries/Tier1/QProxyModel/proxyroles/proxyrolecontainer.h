#ifndef PROXYROLECONTAINER_H
#define PROXYROLECONTAINER_H

#include <QList>
#include <QQmlListProperty>
#include <QQmlEngine>

class QQmlSortFilterProxyModel;

namespace QQsfpm {

class ProxyRole;

class ProxyRoleContainer {
public:
    virtual ~ProxyRoleContainer() = default;

    const QList<ProxyRole*>& proxyRoles() const;
    void appendProxyRole(ProxyRole* proxyRole);
    void removeProxyRole(ProxyRole* proxyRole);
    void clearProxyRoles();

    QQmlListProperty<QQsfpm::ProxyRole> proxyRolesListProperty();

protected:
    QList<ProxyRole*> m_proxyRoles;

private:
    virtual void onProxyRoleAppended(ProxyRole* proxyRole) = 0;
    virtual void onProxyRoleRemoved(ProxyRole* proxyRole) = 0;
    virtual void onProxyRolesCleared() = 0;

#if QT_VERSION_MAJOR < 6
    using q_size_type = int;
#else
    using q_size_type = qsizetype;
#endif
    static void append_proxyRole(QQmlListProperty<QQsfpm::ProxyRole>* list, ProxyRole* proxyRole);
    static q_size_type count_proxyRole(QQmlListProperty<QQsfpm::ProxyRole>* list);
    static ProxyRole* at_proxyRole(QQmlListProperty<QQsfpm::ProxyRole>* list, q_size_type index);
    static void clear_proxyRoles(QQmlListProperty<QQsfpm::ProxyRole>* list);
};

}

#define ProxyRoleContainer_iid "fr.grecko.SortFilterProxyModel.ProxyRoleContainer"
Q_DECLARE_INTERFACE(QQsfpm::ProxyRoleContainer, ProxyRoleContainer_iid)

#endif // PROXYROLECONTAINER_H
