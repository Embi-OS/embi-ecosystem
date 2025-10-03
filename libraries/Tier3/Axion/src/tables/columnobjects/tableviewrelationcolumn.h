#ifndef TABLEVIEWRELATIONCOLUMN_H
#define TABLEVIEWRELATIONCOLUMN_H

#include "qproxymodel.h"

#include "tableviewcolumn.h"
class TableViewRelationColumn: public TableViewColumn
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, keyRoleName, KeyRoleName, "")
    Q_WRITABLE_VAR_PROPERTY(int, keyRole, KeyRole, Qt::DisplayRole)
    Q_WRITABLE_PTR_PROPERTY(QAbstractItemModel, model, Model, nullptr)
    Q_WRITABLE_REF_PROPERTY(QString, relationRoleName, RelationRoleName, "")
    Q_WRITABLE_VAR_PROPERTY(int, relationRole, RelationRole, Qt::DisplayRole)

public:
    explicit TableViewRelationColumn(QObject* parent = nullptr);

    void viewModelCompleted(const TableViewModel& viewModel) override;

    QVariant data(int row, const TableViewModel& viewModel) final override;

protected:
    void onModelAboutToChanged(QAbstractItemModel* oldModel, QAbstractItemModel* newModel);
    void onModelChanged(QAbstractItemModel* model);
    void updateModelRoles();

private:
    QProxyModel* m_proxyModel = nullptr;
};

#endif // TABLEVIEWRELATIONCOLUMN_H
