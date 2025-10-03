#ifndef TABLEVIEWCOLUMNMODEL_H
#define TABLEVIEWCOLUMNMODEL_H

#include <QModels>
#include <QDefs>

#include "columnobjects/tableviewcolumn.h"

class TableViewColumnModel : public QObjectListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, roleNameSeparator, RoleNameSeparator, ".")

    Q_PROPERTY(QStringList activeRoleNames READ getActiveRoleNames NOTIFY activeRoleNamesChanged FINAL)

public:
    explicit TableViewColumnModel(QObject* parent = nullptr);

    QStringList getActiveRoleNames() const;
    Q_INVOKABLE QString getColumnRoleName(int column, bool full=false) const;

    Q_INVOKABLE bool isColumnSortable(int column) const;
    Q_INVOKABLE bool isColumnFilterable(int column) const;

signals:
    void activeRoleNamesChanged();
    void columnVisibleChanged(int column, bool visible);
    void columnWidthChanged(int column, int width);

private slots:
    void onColumnVisibleChanged(bool visible);
    void onColumnEssentialChanged(bool visible);
    void onColumnWidthChanged(int width);
};

#endif // TABLEVIEWCOLUMNMODEL_H
