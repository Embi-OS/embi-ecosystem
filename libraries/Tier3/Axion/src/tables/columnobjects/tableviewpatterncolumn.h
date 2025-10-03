#ifndef TABLEVIEWPATTERNCOLUMN_H
#define TABLEVIEWPATTERNCOLUMN_H

#include "tableviewcolumn.h"
class TableViewPatternColumn: public TableViewColumn
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, pattern, Pattern, {})

public:
    explicit TableViewPatternColumn(QObject* parent = nullptr);

    void updateRoles(const TableViewModel& viewModel) final override;

    QVariant data(int row, const TableViewModel& viewModel) final override;

private:
    QString m_string;
    QStringList m_captures;
    QStringList m_roleNames;
    QList<QStringList> m_subRoleNamesNames;
    QList<int> m_roles;
    QList<int> m_precisions;
};

#endif // TABLEVIEWPATTERNCOLUMN_H
