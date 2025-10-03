#ifndef TABLEVIEWRESULTCOLUMN_H
#define TABLEVIEWRESULTCOLUMN_H

#include "tableviewcolumn.h"
class TableViewResultColumn: public TableViewColumn
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, resultRoleName, ResultRoleName, "")
    Q_READONLY_REF_PROPERTY(QStringList, resultSubRoleNames, ResultSubRoleNames, {})
    Q_WRITABLE_VAR_PROPERTY(int, resultRole, ResultRole, -1)
    Q_WRITABLE_REF_PROPERTY(QColor, colorDefault, ColorDefault, {})
    Q_WRITABLE_REF_PROPERTY(QColor, colorSuccess, ColorSuccess, {})
    Q_WRITABLE_REF_PROPERTY(QColor, colorFailure, ColorFailure, {})

public:
    explicit TableViewResultColumn(QObject* parent = nullptr);

    void updateRoles(const TableViewModel& viewModel) final override;

    QColor color(int row, const TableViewModel& viewModel) final override;
    QVariant data(int row, const TableViewModel& viewModel) final override;

protected:
    QVariant sourceResultData(int row, const TableViewModel& viewModel);
};

#endif // TABLEVIEWRESULTCOLUMN_H
