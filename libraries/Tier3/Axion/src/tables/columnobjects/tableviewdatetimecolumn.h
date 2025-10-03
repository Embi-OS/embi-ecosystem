#ifndef TABLEVIEWDATETIMECOLUMN_H
#define TABLEVIEWDATETIMECOLUMN_H

#include "tableviewcolumn.h"
class TableViewDateTimeColumn: public TableViewColumn
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, format, Format, {})
    Q_WRITABLE_VAR_PROPERTY(QLocale::FormatType, formatType, FormatType, QLocale::ShortFormat)

public:
    explicit TableViewDateTimeColumn(QObject* parent = nullptr);

    QVariant data(int row, const TableViewModel& viewModel) final override;
};

#endif // TABLEVIEWDATETIMECOLUMN_H
