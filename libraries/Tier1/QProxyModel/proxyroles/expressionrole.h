#ifndef EXPRESSIONROLE_H
#define EXPRESSIONROLE_H

#include "singlerole.h"
#include <QQmlScriptString>

class QQmlExpression;

namespace QQsfpm {

class ExpressionRole : public QQsfpm::SingleRole
{
    Q_OBJECT

    Q_WRITABLE_REF_PROPERTY(QQmlScriptString, expression, Expression, {})
    Q_WRITABLE_REF_PROPERTY(QStringList, customRoleNames, CustomRoleNames, {})

public:
    explicit ExpressionRole(QObject *parent = nullptr);

    void proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel) override;
    void updateRoles(const QQmlSortFilterProxyModel& proxyModel) override;

protected slots:
    void updateContext(const QQmlSortFilterProxyModel& proxyModel);
    void updateExpression();

private:
    QVariant data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) override;

    QHash<int, QByteArray> m_roleNames;
    QQmlExpression* m_qmlExpression = nullptr;
    QQmlContext* m_context = nullptr;
};

}

#endif // EXPRESSIONROLE_H
