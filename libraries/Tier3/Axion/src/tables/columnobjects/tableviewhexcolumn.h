#ifndef TABLEVIEWHEXCOLUMN_H
#define TABLEVIEWHEXCOLUMN_H

#include "tableviewcolumn.h"
class TableViewHexColumn: public TableViewColumn
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit TableViewHexColumn(QObject* parent = nullptr);

    QVariant data(int row, const TableViewModel& viewModel) final override;
};

#endif // TABLEVIEWHEXCOLUMN_H
