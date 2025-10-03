#ifndef RELATIONROLE_H
#define RELATIONROLE_H

#include "singlerole.h"
#include <QAbstractItemModel>
#include "qproxymodel.h"

namespace QQsfpm {

class RelationRole : public QQsfpm::SingleRole
{
    Q_OBJECT

    Q_WRITABLE_VAR_PROPERTY(bool, delayed, Delayed, false)
    Q_WRITABLE_REF_PROPERTY(QString, roleName, RoleName, {})
    Q_WRITABLE_VAR_PROPERTY(int, role, Role, -1)
    Q_WRITABLE_REF_PROPERTY(QString, modelRoleName, ModelRoleName, {})
    Q_WRITABLE_VAR_PROPERTY(int, modelRole, ModelRole, -1)
    Q_WRITABLE_PTR_PROPERTY(QAbstractItemModel, model, Model, nullptr)
    Q_WRITABLE_REF_PROPERTY(QStringList, customRoleNames, CustomRoleNames, {})
    Q_WRITABLE_REF_PROPERTY(QVariantMap, defaultObject, DefaultObject, {})

public:
    explicit RelationRole(QObject *parent = nullptr);

    void proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel) override;
    void updateRoles(const QQmlSortFilterProxyModel& proxyModel) override;

private slots:
    void onModelAboutToChange(QAbstractItemModel* oldModel, QAbstractItemModel* newModel);
    void onModelChanged(QAbstractItemModel* model);

private:
    QVariant data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) override;

    QProxyModel* m_proxyModel = nullptr;
    QHash<int, QByteArray> m_proxyRoleNames;
    QHash<int, QVariant> m_proxyDefaultObject;
};

}

#endif // RELATIONROLE_H
