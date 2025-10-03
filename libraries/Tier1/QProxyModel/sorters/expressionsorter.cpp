#include "expressionsorter.h"
#include "qqmlsortfilterproxymodel.h"

#include <QQmlContext>
#include <QQmlExpression>

namespace QQsfpm {

/*!
    \qmltype ExpressionSorter
    \inherits Sorter
    \inqmlmodule SortFilterProxyModel
    \ingroup Sorters
    \brief Sorts row with a custom javascript expression.

    An ExpressionSorter is a \l Sorter allowing to implement custom sorting based on a javascript expression.
*/

ExpressionSorter::ExpressionSorter(QObject *parent) : Sorter(parent)
{
    connect(this, &ExpressionSorter::expressionChanged, this, &ExpressionSorter::updateExpression);
}

void ExpressionSorter::proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel)
{
    updateContext(proxyModel);
}

bool evaluateBoolExpression(QQmlExpression& expression)
{
    QVariant variantResult = expression.evaluate();
    if (expression.hasError()) {
        qWarning()<<expression.error();
        return false;
    }
    if (variantResult.canConvert<bool>()) {
        return variantResult.toBool();
    } else {
        qWarning("%s:%i:%i : Can't convert result to bool",
                 expression.sourceFile().toUtf8().data(),
                 expression.lineNumber(),
                 expression.columnNumber());
        return false;
    }
}

int ExpressionSorter::compare(const QModelIndex& sourceLeft, const QModelIndex& sourceRight, const QQmlSortFilterProxyModel& proxyModel) const
{
    if (!m_expression.isEmpty()) {
        QVariantMap modelLeftMap, modelRightMap;
        QHash<int, QByteArray> roles = proxyModel.roleNames();

        QQmlContext context(qmlContext(this));

        for (auto it = roles.cbegin(); it != roles.cend(); ++it) {
            modelLeftMap.insert(it.value(), proxyModel.sourceData(sourceLeft, it.key()));
            modelRightMap.insert(it.value(), proxyModel.sourceData(sourceRight, it.key()));
        }
        modelLeftMap.insert("index", sourceLeft.row());
        modelRightMap.insert("index", sourceRight.row());

        QQmlExpression expression(m_expression, &context);

        context.setContextProperty("modelLeft", modelLeftMap);
        context.setContextProperty("modelRight", modelRightMap);
        if (evaluateBoolExpression(expression))
                return -1;

        context.setContextProperty("modelLeft", modelRightMap);
        context.setContextProperty("modelRight", modelLeftMap);
        if (evaluateBoolExpression(expression))
                return 1;
    }
    return 0;
}

void ExpressionSorter::updateContext(const QQmlSortFilterProxyModel& proxyModel)
{
    delete m_context;
    m_context = new QQmlContext(qmlContext(this), this);

    QVariantMap modelLeftMap, modelRightMap;
    // what about roles changes ?

    QHash<int, QByteArray> names = proxyModel.roleNames();
    for (QHash<int, QByteArray>::iterator it = names.begin(); it != names.end(); ++it) {
        modelLeftMap.insert(it.value(), QVariant());
        modelRightMap.insert(it.value(), QVariant());
    }
    modelLeftMap.insert("index", -1);
    modelRightMap.insert("index", -1);

    m_context->setContextProperty("modelLeft", modelLeftMap);
    m_context->setContextProperty("modelRight", modelRightMap);

    updateExpression();
}

void ExpressionSorter::updateExpression()
{
    if (!m_context || !m_enabled)
        return;

    delete m_qmlExpression;
    m_qmlExpression = new QQmlExpression(m_expression, m_context, 0, this);
    connect(m_qmlExpression, &QQmlExpression::valueChanged, this, &ExpressionSorter::invalidate);
    m_qmlExpression->setNotifyOnValueChanged(true);
    m_qmlExpression->evaluate();

    invalidate();
}

}
