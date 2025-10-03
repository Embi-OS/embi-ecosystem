#include "expressionrole.h"
#include "qqmlsortfilterproxymodel.h"

#include <QQmlContext>
#include <QQmlExpression>

namespace QQsfpm {

/*!
    \qmltype ExpressionRole
    \inherits SingleRole
    \inqmlmodule SortFilterProxyModel
    \ingroup ProxyRoles
    \brief A custom role computed from a javascript expression.

    An ExpressionRole is a \l ProxyRole allowing to implement a custom role based on a javascript expression.

    In the following example, the \c c role is computed by adding the \c a role and \c b role of the model :
    \code
    SortFilterProxyModel {
       sourceModel: numberModel
       proxyRoles: ExpressionRole {
           name: "c"
           expression: model.a + model.b
      }
    }
    \endcode
*/

ExpressionRole::ExpressionRole(QObject *parent) : SingleRole(parent)
{
    connect(this, &ExpressionRole::expressionChanged, this, &ExpressionRole::updateExpression);
}

void ExpressionRole::proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel)
{
    updateContext(proxyModel);
}

void ExpressionRole::updateRoles(const QQmlSortFilterProxyModel& proxyModel)
{
    m_roleNames = proxyModel.roleNames();
    if(!m_customRoleNames.isEmpty())
    {
        const QHash<int,QByteArray> roleNames = m_roleNames;
        m_roleNames.clear();
        m_roleNames.reserve(m_customRoleNames.size());
        for(const QString& customRoleName: m_customRoleNames)
        {
            const QByteArray roleName = customRoleName.toUtf8();
            m_roleNames[roleNames.key(roleName)] = roleName;
        }
    }
}

QVariant ExpressionRole::data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel)
{
    if (m_expression.isEmpty())
        return QVariant();

    QVariantMap modelMap;
    QQmlContext context(qmlContext(this));

    if(!context.isValid()) {
        qWarning()<<"invalid context";
        return m_defaultValue;
    }

    auto addToContext = [&] (const QString &name, const QVariant& value) {
        // context.setContextProperty(name, value);
        modelMap.insert(name, value);
    };

    for(auto [key, value]: m_roleNames.asKeyValueRange())
        addToContext(value, proxyModel.sourceData(sourceIndex, key));
    addToContext("index", sourceIndex.row());

    context.setContextProperty("model", modelMap);

    QQmlExpression expression(m_expression, &context);
    const QVariant result = expression.evaluate();

    if (expression.hasError()) {
        qWarning()<<expression.error();
        return m_defaultValue;
    }

    return result;
}

void ExpressionRole::updateContext(const QQmlSortFilterProxyModel& proxyModel)
{
    delete m_context;
    m_context = new QQmlContext(qmlContext(this), this);
    // what about roles changes ?
    QVariantMap modelMap;

    auto addToContext = [&] (const QString &name, const QVariant& value) {
        m_context->setContextProperty(name, value);
        modelMap.insert(name, value);
    };

    QHash<int, QByteArray> names = proxyModel.roleNames();
    for (QHash<int, QByteArray>::iterator it = names.begin(); it != names.end(); ++it)
        addToContext(it.value(), QVariant());

    addToContext("index", -1);

    m_context->setContextProperty("model", modelMap);
    updateExpression();
}

void ExpressionRole::updateExpression()
{
    if (!m_context || !m_enabled)
        return;

    delete m_qmlExpression;
    m_qmlExpression = new QQmlExpression(m_expression, m_context, 0, this);
    connect(m_qmlExpression, &QQmlExpression::valueChanged, this, &ExpressionRole::invalidate);
    m_qmlExpression->setNotifyOnValueChanged(true);
    m_qmlExpression->evaluate();

    if (m_qmlExpression->hasError()) {
        qWarning()<<m_qmlExpression->error();
    }

    invalidate();
}

}
