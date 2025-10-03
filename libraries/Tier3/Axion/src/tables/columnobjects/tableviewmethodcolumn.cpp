#include "tableviewmethodcolumn.h"
#include "tableviewmodel.h"
#include "axion_log.h"

#include <QQmlContext>
#include <QQmlExpression>

TableViewMethodColumn::TableViewMethodColumn(QObject* parent):
    TableViewColumn(parent)
{

}

void TableViewMethodColumn::componentComplete()
{
    updateMethod();
}

QVariant TableViewMethodColumn::data(int row, const TableViewModel& viewModel)
{
    const QModelIndex modelIndex = viewModel.index(row,viewModel.getColumnModel()->indexOf(this));

    if (m_cached && m_cache.contains(modelIndex)) {
        return m_cache.value(modelIndex);
    }

    QVariant params = sourceData(row, viewModel);
    if(m_parameterType.id()!=QMetaType::QVariant && !params.canConvert(m_parameterType)) {
        AXIONLOG_WARNING()<<"Unable to convert params type"<<params.metaType()<<"to"<<m_parameterType;
        return m_defaultValue;
    }
    else if(m_parameterType.id()!=QMetaType::QVariant) {
        params.convert(m_parameterType);
    }

    if(!m_method.isValid())
        return m_defaultValue;

    QVariant ret(m_returnType);
    void *argv[] = {&ret, params.data()};
    QMetaObject::metacall(const_cast<TableViewMethodColumn*>(this), QMetaObject::InvokeMetaMethod, m_method.methodIndex(), argv);

    if (m_cached) {
        m_cache[modelIndex] = ret;
    }

    m_cache[modelIndex] = ret;

    return ret;
}

void TableViewMethodColumn::updateMethod()
{
    const auto *metaObj = metaObject();
    for (int idx = metaObj->methodOffset(); idx < metaObj->methodCount(); idx++) {
        // Once we find the method signature, break the loop
        QMetaMethod method = metaObj->method(idx);
        if (method.name() == "method") {
            m_method = method;
            break;
        }
    }

    if (!m_method.isValid())
        return;

    if (m_method.parameterCount() != 1) {
        qWarning()<<"TableViewMethodColumn method requires a single parameter";
        return;
    }

    m_parameterType = m_method.parameterMetaType(0);
    m_returnType = m_method.returnMetaType();

    invalidate();
}

void TableViewMethodColumn::invalidateCache()
{
    m_cache.clear();
}
