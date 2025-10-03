#ifndef METHODROLE_H
#define METHODROLE_H

#include "singlerole.h"
#include <QMetaMethod>

namespace QQsfpm {

class MethodRole : public QQsfpm::SingleRole
{
    Q_OBJECT

    Q_WRITABLE_REF_PROPERTY(QStringList, customRoleNames, CustomRoleNames, {})

public:
    explicit MethodRole(QObject *parent = nullptr);

    void componentComplete() override;

    void updateRoles(const QQmlSortFilterProxyModel& proxyModel) override;

protected slots:
    void updateMethod();

private:
    QVariant data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) override;

    QHash<int, QByteArray> m_roleNames;
    QMetaMethod m_method;
    QList<QByteArray> m_parameterNames;
    QList<int> m_parameterRoles;
    QList<QMetaType> m_parameterTypes;
    QMetaType m_returnType;
};

}

#endif // METHODROLE_H
