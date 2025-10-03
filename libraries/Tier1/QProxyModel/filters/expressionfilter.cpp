#include "expressionfilter.h"
#include "qqmlsortfilterproxymodel.h"

#include <QQmlContext>
#include <QQmlExpression>

namespace QQsfpm {

/*!
    \qmltype ExpressionFilter
    \inherits Filter
    \inqmlmodule SortFilterProxyModel
    \ingroup Filters
    \brief Filters row with a custom filtering.

    An ExpressionFilter is a \l Filter allowing to implement custom filtering based on a javascript expression.
*/

ExpressionFilter::ExpressionFilter(QObject *parent) : Filter(parent)
{
    connect(this, &ExpressionFilter::expressionChanged, this, &ExpressionFilter::updateExpression);
}

void ExpressionFilter::proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel)
{
    updateContext(proxyModel);
}

bool ExpressionFilter::filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const
{
    if (!m_expression.isEmpty()) {
        QVariantMap modelMap;
        QHash<int, QByteArray> roles = proxyModel.roleNames();

        QQmlContext context(qmlContext(this));

        if(!context.isValid())
            return false;

        auto addToContext = [&] (const QString &name, const QVariant& value) {
            context.setContextProperty(name, value);
            modelMap.insert(name, value);
        };

        for (auto it = roles.cbegin(); it != roles.cend(); ++it)
            addToContext(it.value(), proxyModel.sourceData(sourceIndex, it.key()));
        addToContext("index", sourceIndex.row());

        context.setContextProperty("model", modelMap);

        QQmlExpression expression(m_expression, &context);
        QVariant variantResult = expression.evaluate();

        if (expression.hasError()) {
            qWarning()<<expression.error();
            return true;
        }
        if (variantResult.canConvert<bool>()) {
            return variantResult.toBool();
        } else {
            qWarning("%s:%i:%i : Can't convert result to bool",
                     expression.sourceFile().toUtf8().data(),
                     expression.lineNumber(),
                     expression.columnNumber());
            return true;
        }
    }
    return true;
}

void ExpressionFilter::updateContext(const QQmlSortFilterProxyModel& proxyModel)
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

void ExpressionFilter::updateExpression()
{
    if (!m_context || !m_enabled)
        return;

    delete m_qmlExpression;
    m_qmlExpression = new QQmlExpression(m_expression, m_context, 0, this);
    connect(m_qmlExpression, &QQmlExpression::valueChanged, this, &ExpressionFilter::invalidate);
    m_qmlExpression->setNotifyOnValueChanged(true);
    m_qmlExpression->evaluate();

    invalidate();
}

}
