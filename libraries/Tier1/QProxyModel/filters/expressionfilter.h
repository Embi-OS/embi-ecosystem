#ifndef EXPRESSIONFILTER_H
#define EXPRESSIONFILTER_H

#include "filter.h"
#include <QQmlScriptString>

class QQmlExpression;

namespace QQsfpm {

class ExpressionFilter : public QQsfpm::Filter
{
    Q_OBJECT

    Q_WRITABLE_REF_PROPERTY(QQmlScriptString, expression, Expression, {})

public:
    explicit ExpressionFilter(QObject *parent = nullptr);

    void proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel) override;

protected:
    bool filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const override;

protected slots:
    void updateContext(const QQmlSortFilterProxyModel& proxyModel);
    void updateExpression();

private:
    QQmlExpression* m_qmlExpression = nullptr;
    QQmlContext* m_context = nullptr;
};

}

#endif // EXPRESSIONFILTER_H
