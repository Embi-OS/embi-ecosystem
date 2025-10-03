#ifndef TABLEVIEWLISTCOLUMN_H
#define TABLEVIEWLISTCOLUMN_H

#include "tableviewcolumn.h"
class TableViewListColumn: public TableViewColumn
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, separator, Separator, "; ")

public:
    explicit TableViewListColumn(QObject* parent = nullptr);

    QVariant data(int row, const TableViewModel& viewModel) final override;
};

#endif // TABLEVIEWLISTCOLUMN_H
