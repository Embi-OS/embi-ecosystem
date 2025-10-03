#ifndef EXPRESSIONSORTER_H
#define EXPRESSIONSORTER_H

#include "sorter.h"
#include <QQmlScriptString>

class QQmlExpression;

namespace QQsfpm {

class ExpressionSorter : public QQsfpm::Sorter
{
    Q_OBJECT

    Q_WRITABLE_REF_PROPERTY(QQmlScriptString, expression, Expression, {})

public:
    explicit ExpressionSorter(QObject *parent = nullptr);

    void proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel) override;

protected:
    int compare(const QModelIndex& sourceLeft, const QModelIndex& sourceRight, const QQmlSortFilterProxyModel& proxyModel) const override;

protected slots:
    void updateContext(const QQmlSortFilterProxyModel& proxyModel);
    void updateExpression();

private:
    QQmlExpression* m_qmlExpression = nullptr;
    QQmlContext* m_context = nullptr;
};

}

#endif // EXPRESSIONSORTER_H
