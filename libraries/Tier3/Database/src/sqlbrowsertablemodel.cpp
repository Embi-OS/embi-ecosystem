#include "sqlbrowsertablemodel.h"

SqlBrowserTableModel::SqlBrowserTableModel(QObject *parent, QSqlDatabase db):
    QSqlTableModel(parent, db)
{
    m_roleNames = QHash<int, QByteArray> {
        { DisplayRole, QByteArrayLiteral("display") },
        { EditRole, QByteArrayLiteral("edit") },
        { DirtyRole, QByteArrayLiteral("dirty") },
        { FieldnameRole, QByteArrayLiteral("fieldname") },
        { TypeRole, QByteArrayLiteral("type") },
        { LengthRole, QByteArrayLiteral("length") },
        { PrecisionRole, QByteArrayLiteral("precision") },
        { RequiredRole, QByteArrayLiteral("required") },
        { AutoValueRole, QByteArrayLiteral("autoValue") },
        { DefaultValueRole, QByteArrayLiteral("defaultValue") }
    };

    setEditStrategy(OnManualSubmit);
}

QVariant SqlBrowserTableModel::data(const QModelIndex &index, int role) const
{
    if (role == DisplayRole || role == EditRole)
        return QSqlTableModel::data(index, role);
    else if (role == DirtyRole)
        return isDirty(index);
    else
    {
        QSqlRecord rec = record(index.row());
        QSqlField fld = rec.field(index.column());
        if (role == FieldnameRole)
            return fld.name();
        else if (role == TypeRole)
            return dataType(fld.metaType().name());
        else if (role == LengthRole)
            return fld.length();
        else if (role == PrecisionRole)
            return fld.precision();
        else if (role == RequiredRole)
            return fld.requiredStatus() == -1 ? QVariant("?") : QVariant(bool(fld.requiredStatus()));
        else if (role == AutoValueRole)
            return fld.isAutoValue();
        else if (role == DefaultValueRole)
            return fld.defaultValue();
    }
    return QSqlTableModel::data(index, role);
}

QHash<int, QByteArray> SqlBrowserTableModel::roleNames() const
{
    return m_roleNames;
}

bool SqlBrowserTableModel::hasChanged() const
{
    return isDirty();
}

SqlBrowserDataTypes::Enum SqlBrowserTableModel::dataType(const QString& metaType)
{
    if(metaType=="char")
        return SqlBrowserDataTypes::Boolean;
    if(metaType=="bool")
        return SqlBrowserDataTypes::Boolean;
    if(metaType=="QByteArray")
        return SqlBrowserDataTypes::Bytes;
    if(metaType=="double")
        return SqlBrowserDataTypes::Number;
    if(metaType=="int")
        return SqlBrowserDataTypes::Number;

    return SqlBrowserDataTypes::Text;
}

bool SqlBrowserTableModel::select()
{
    bool result = QSqlTableModel::select();
    while(canFetchMore())
        fetchMore();
    return result;
}
