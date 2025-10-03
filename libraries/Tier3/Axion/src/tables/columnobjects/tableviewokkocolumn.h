#ifndef TABLEVIEWOKKOCOLUMN_H
#define TABLEVIEWOKKOCOLUMN_H

#include "tableviewcolumn.h"
class TableViewOkKoColumn: public TableViewColumn
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, okkoRoleName, OKKORoleName, "")
    Q_READONLY_REF_PROPERTY(QStringList, okkoSubRoleNames, OKKOSubRoleNames, {})
    Q_WRITABLE_VAR_PROPERTY(int, okkoRole, OKKORole, -1)
    Q_WRITABLE_REF_PROPERTY(QColor, colorOk, ColorOk, {})
    Q_WRITABLE_REF_PROPERTY(QColor, colorKo, ColorKo, {})

public:
    explicit TableViewOkKoColumn(QObject* parent = nullptr);

    void updateRoles(const TableViewModel& viewModel) final override;

    QColor color(int row, const TableViewModel& viewModel) final override;
    QVariant data(int row, const TableViewModel& viewModel) final override;

protected:
    QVariant sourceOKKOData(int row, const TableViewModel& viewModel);
};

#endif // TABLEVIEWOKKOCOLUMN_H
