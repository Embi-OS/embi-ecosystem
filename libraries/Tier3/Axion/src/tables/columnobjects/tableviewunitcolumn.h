#ifndef TABLEVIEWUNITCOLUMN_H
#define TABLEVIEWUNITCOLUMN_H

#include <Unit>
#include "tableviewcolumn.h"
class TableViewUnitColumn: public TableViewColumn
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, unitRoleName, UnitRoleName, "")
    Q_READONLY_REF_PROPERTY(QStringList, unitSubRoleNames, UnitSubRoleNames, {})
    Q_WRITABLE_VAR_PROPERTY(int, unitRole, UnitRole, Qt::DisplayRole)
    Q_WRITABLE_PTR_PROPERTY(UnitCategoryObject, unitModel, UnitModel, nullptr)
    Q_WRITABLE_VAR_PROPERTY(bool, showUnitType, ShowUnitType, true)

public:
    explicit TableViewUnitColumn(QObject* parent = nullptr);

    void updateRoles(const TableViewModel& viewModel) final override;

    QVariant data(int row, const TableViewModel& viewModel) final override;

protected:
    QVariant sourceUnitData(int row, const TableViewModel& viewModel);
};

#endif // TABLEVIEWUNITCOLUMN_H
