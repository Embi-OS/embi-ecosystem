#ifndef TABLEVIEWMETHODCOLUMN_H
#define TABLEVIEWMETHODCOLUMN_H

#include "tableviewcolumn.h"
class TableViewMethodColumn: public TableViewColumn
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_VAR_PROPERTY(bool, cached, Cached, false)

public:
    explicit TableViewMethodColumn(QObject* parent = nullptr);

    void componentComplete() override;

    QVariant data(int row, const TableViewModel& viewModel) final override;

protected slots:
    void updateMethod();
    void invalidateCache();

private:
    QMetaMethod m_method;
    QMetaType m_parameterType;
    QMetaType m_returnType;
    QHash<QPersistentModelIndex, QVariant> m_cache;
};

#endif // TABLEVIEWMETHODCOLUMN_H
