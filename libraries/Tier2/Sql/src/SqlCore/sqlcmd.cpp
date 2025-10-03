#include "sqlcmd.h"
#include "sqldbpool.h"
#include "sql_log.h"

QVariantMap Sql::m_filterMap = QVariantMap();
void Sql::fillFilterMap() {
    m_filterMap.insert("eq", SqlFilterOperators::Equals);
    m_filterMap.insert("neq", SqlFilterOperators::NotEquals);
    m_filterMap.insert("lt", SqlFilterOperators::LessThan);
    m_filterMap.insert("lte", SqlFilterOperators::LessEquals);
    m_filterMap.insert("gt", SqlFilterOperators::GreaterThan);
    m_filterMap.insert("gte", SqlFilterOperators::GreaterEquals);
    m_filterMap.insert("range", SqlFilterOperators::Range);
    m_filterMap.insert("in", SqlFilterOperators::In);
    m_filterMap.insert("icontains", SqlFilterOperators::IContains);
    m_filterMap.insert("istartswith", SqlFilterOperators::IStartsWith);
    m_filterMap.insert("iendswith", SqlFilterOperators::IEndsWith);
    m_filterMap.insert("contains", SqlFilterOperators::Contains);
    m_filterMap.insert("startswith", SqlFilterOperators::StartsWith);
    m_filterMap.insert("endswith", SqlFilterOperators::EndsWith);
    m_filterMap.insert("regex", SqlFilterOperators::RegExp);
}

Sql::Sql(QObject *parent) :
    QObject(parent)
{

}

bool Sql::isIfNullValid(const QString& name)
{
    if(name.toUpper()=="IFNULL")
        return true;
    return false;
}

bool Sql::isValueTypeValid(const QString& name)
{
    if(name.toUpper()=="BINARY")
        return true;
    else if(name.toUpper()=="BOOLEAN")
        return true;
    else if(name.toUpper()=="UNSIGNED")
        return true;
    else if(name.toUpper()=="DOUBLE")
        return true;
    else if(name.toUpper()=="INTEGER")
        return true;
    else if(name.toUpper()=="DATE")
        return true;
    return false;
}

bool Sql::isFilterNameValid(const QString& name)
{
    if(m_filterMap.isEmpty())
        fillFilterMap();
    return m_filterMap.contains(name);
}

QString Sql::filterName(SqlFilterOperators::Enum op)
{
    if(m_filterMap.isEmpty())
        fillFilterMap();
    return m_filterMap.key(op, "");
}

SqlFilterOperators::Enum Sql::filterOperator(const QString& name)
{
    if(m_filterMap.isEmpty())
        fillFilterMap();
    return (SqlFilterOperators::Enum)m_filterMap.value(name, SqlFilterOperators::Equals).toInt();
}

QString Sql::filterEquals(const QString &tableName, const QString &columnName, const QVariant &value, const QSqlDriver *driver, bool preparedStatement)
{
    if(!driver)
        driver = SqlDbPool::defaultDriver();

    if(!driver)
    {
        SQLLOG_WARNING()<<"No driver has been given";
        return QString();
    }

    const auto tableNameString = tableName.isEmpty() ? QString() : prepareIdentifier(tableName, QSqlDriver::TableName, driver);
    const QString tableNamePrefix = tableNameString.isEmpty() ? QString() : (tableNameString + QLatin1Char('.'));
    const QString columnNamePrefix = prepareIdentifier(columnName, QSqlDriver::FieldName, driver);

    QString filter;
    filter.append(tableNamePrefix);
    filter.append(columnNamePrefix);
    if (value.isNull())
        filter.append(QLatin1String(" IS NULL"));
    else if (preparedStatement)
        filter.append(QLatin1String(" = ?"));
    else {
        const QString whereVal = formatValue(value, driver);
        if(whereVal.startsWith('(') && whereVal.endsWith(')'))
            filter.append(QLatin1String(" IN ")).append(whereVal);
        else if(!whereVal.isEmpty())
            filter.append(QLatin1String(" = ")).append(whereVal);
        else
            filter.clear();
    }

    return filter;
}

QString Sql::prepareIdentifier(const QString &identifier, QSqlDriver::IdentifierType type, const QSqlDriver *driver)
{
    if(!driver)
        driver = SqlDbPool::defaultDriver();

    if(!driver)
    {
        SQLLOG_WARNING()<<"No driver has been given";
        return QString();
    }

    QString ret = identifier;
    if(type==QSqlDriver::FieldName && identifier.contains("__"))
    {
        QStringList fields = identifier.split("__");
        QString valueType;
        bool ifNull = false;
        if(Sql::isValueTypeValid(fields.last())) {
            valueType = fields.takeLast().toUpper();
        }
        if(Sql::isIfNullValid(fields.last())) {
            ifNull = true;
            fields.takeLast();
        }
        QString column = fields.takeFirst();
        if(!fields.isEmpty())
        {
            if(fields.contains("idx_start") && fields.contains("idx_end"))
            {
                // We want to handle dynamic array indices
                // e.g. program__details__instructions__idx_start__details__instruction_index__idx_end__name
                //                      column             array                                indexColumn index                   field
                //      -> JSON_EXTRACT(program, CONCAT('$.details.instructions[', JSON_EXTRACT(details, '$.instruction_index'), '].name'))

                int idxStart = fields.indexOf("idx_start");
                int idxEnd = fields.indexOf("idx_end");

                QStringList arrayFields = fields.mid(0, idxStart);
                QStringList indexFields = fields.mid(idxStart+1, idxEnd-idxStart-1);
                QString indexColumn = indexFields.takeFirst();
                QStringList fieldFields = fields.mid(idxEnd+1);

                indexFields.prepend("$");
                QString indexPath = indexFields.join(".");
                QString indexExtract = QString("JSON_EXTRACT(%1, %2)").arg(indexColumn, formatValue(indexPath, driver));

                fieldFields.prepend("]");
                QString fieldPath = fieldFields.join(".");

                arrayFields.prepend("$");
                arrayFields.last().append("[");
                QString arrayPath = arrayFields.join(".");
                QString concat = QString("CONCAT(%1, %2, %3)").arg(formatValue(arrayPath, driver), indexExtract, formatValue(fieldPath, driver));

                ret = QString("JSON_EXTRACT(%1, %2)").arg(column, concat);
            }
            else
            {
                fields.prepend("$");
                QString jsonPath = fields.join(".");
                if(!driver->isIdentifierEscaped(column, type))
                    column = driver->escapeIdentifier(column, type);
                ret = QString("JSON_EXTRACT(%1, %2)").arg(column, formatValue(jsonPath, driver));
            }

            if(driver->dbmsType()!=QSqlDriver::SQLite && valueType.isEmpty())
                ret = QString("JSON_UNQUOTE(%1)").arg(ret);
        }
        else
        {
            if(!driver->isIdentifierEscaped(column, type))
                ret = driver->escapeIdentifier(column, type);
        }

        if(ifNull)
            ret = QString("IFNULL(%1,%2)").arg(ret,formatValue("", driver));

        if(valueType.toUpper()=="BINARY")
            ret = QString("CAST(%1 as %2)").arg(ret, valueType);
        else if(valueType.toUpper()=="UNSIGNED")
            ret = QString("CAST(%1 as %2)").arg(ret, valueType);
        else if(valueType.toUpper()=="DOUBLE")
            ret = QString("CAST(%1 as %2)").arg(ret, valueType);
        else if(valueType.toUpper()=="INTEGER")
            ret = QString("CAST(%1 as %2)").arg(ret, valueType);
        else if(valueType.toUpper()=="DATE")
            ret = QString("DATE(%1)").arg(ret);
        else if(valueType.toUpper()=="BOOLEAN") {
            const QString trueStr = formatValue("true", driver);
            const QString falseStr = formatValue("false", driver);
            const QString oneStr = formatValue(1, driver);
            const QString zeroStr = formatValue(0, driver);
            ret = QString("(CASE WHEN %1=%2 THEN %4 WHEN %1=%3 THEN %5 ELSE CAST(IFNULL(%1,%5) as UNSIGNED) END)").arg(ret, trueStr, falseStr, oneStr, zeroStr);
        }
    }
    else if (!driver->isIdentifierEscaped(identifier, type))
    {
        ret = driver->escapeIdentifier(identifier, type);
    }

    return ret;
}

QVariant Sql::formatValue(const QVariant& value, SqlColumnTypes::Enum type, bool* ok)
{
    if(value.typeId()==QMetaType::QVariantList)
    {
        const QVariantList values = value.toList();
        QVariantList retVal;
        for(const QVariant& val : values)
            retVal.append(Sql::formatValue(val, type, ok));

        return retVal;
    }

    switch (type) {
    case SqlColumnTypes::Binary:
    case SqlColumnTypes::VarBinary:
    case SqlColumnTypes::Blob:
        if(ok)
            *ok = true;
        return value.toByteArray();
    case SqlColumnTypes::Boolean:
        if(ok)
            *ok = true;
        return value.toBool();
    case SqlColumnTypes::SmallInt:
    case SqlColumnTypes::Integer:
    case SqlColumnTypes::BigInt:
        return value.toLongLong(ok);
    case SqlColumnTypes::Float:
        return value.toFloat(ok);
    case SqlColumnTypes::DoublePrecision:
        return value.toDouble(ok);
    case SqlColumnTypes::Date:
    case SqlColumnTypes::Time:
    case SqlColumnTypes::DateTime:
    case SqlColumnTypes::Timestamp:
    case SqlColumnTypes::Char:
    case SqlColumnTypes::VarChar:
    case SqlColumnTypes::TinyText:
    case SqlColumnTypes::Text:
    case SqlColumnTypes::MediumText:
    case SqlColumnTypes::LongText:
    case SqlColumnTypes::Json:
    case SqlColumnTypes::Uuid:
    default:
        if(ok)
            *ok = true;
        return value.toString();
        break;
    }
}

QString Sql::formatValue(QVariant value, const QSqlDriver *driver, bool trimStrings)
{
    if(!driver)
        driver = SqlDbPool::defaultDriver();

    if(!driver)
    {
        SQLLOG_WARNING()<<"No driver has been given";
        return QString();
    }

    if(value.isNull())
        return QString("NULL");

    if(driver->dbmsType()==QSqlDriver::MySqlServer && value.typeId()==QMetaType::QDateTime)
    {
        if (QDateTime dt = value.toDateTime().toUTC(); dt.isValid()) {
            // MySQL format doesn't like the "Z" at the end, but does allow
            // "+00:00" starting in version 8.0.19. However, if we got here,
            // it's because the MySQL server is too old for prepared queries
            // in the first place, so it won't understand timezones either.
            return u'\'' +
                dt.date().toString(Qt::ISODate) +
                u'T' +
                dt.time().toString(Qt::ISODate) +
                u'\'';
        }
    }
    else if(value.typeId()==QMetaType::QVariantList)
    {
        const QVariantList values = value.toList();
        QStringList whereVal;
        for(const QVariant& val : values)
            whereVal.append(Sql::formatValue(val, driver, trimStrings));

        if(whereVal.isEmpty())
            return QString();
        else if(whereVal.size()==1)
            return whereVal.at(0);
        else
            return QString("(%1)").arg(whereVal.join(','));
    }

    QSqlField field;
    field.setMetaType(value.metaType());
    field.setValue(value);

    return driver->formatValue(field, trimStrings);
}

QString Sql::primaryField(const QString &tableName, const QSqlDriver *driver)
{
    if(!driver)
        driver = SqlDbPool::defaultDriver();

    if(!driver)
    {
        SQLLOG_WARNING()<<"No driver has been given";
        return QString();
    }

    const QSqlIndex primaryIndex = driver->primaryIndex(tableName);
    if(primaryIndex.isEmpty())
        return QString();

    return primaryIndex.fieldName(0);
}

QSqlRecord Sql::prepareRecord(const QString &tableName, const QVariantMap& map, const QSqlDriver *driver)
{
    if(!driver)
    {
        SQLLOG_WARNING()<<"No driver has been given to Sql::prepareRecord";
        return QSqlRecord();
    }

    const QSqlRecord tableRecord = driver->record(tableName);
    QSqlRecord record;

    for(auto [key, value]: map.asKeyValueRange())
    {
        if(tableRecord.contains(key))
        {
            QSqlField field(key);
            field.setMetaType(value.metaType());
            field.setValue(value);
            record.append(field);
        }
    }

    return record;
}

QString Sql::emptyInsertStatement(const QString &tableName, const QSqlDriver *driver, SqlInsertStatementTypes::Enum insertType)
{
    if(!driver)
        driver = SqlDbPool::defaultDriver();

    if(!driver)
    {
        SQLLOG_WARNING()<<"No driver has been given";
        return QString();
    }

    const QSqlRecord tableRecord = driver->record(tableName);
    QString s = driver->sqlStatement(QSqlDriver::InsertStatement, tableName, tableRecord, true);
    QString insert;
    switch(insertType) {
    case SqlInsertStatementTypes::InsertOrReplace:
        if(driver->dbmsType()==QSqlDriver::SQLite)
        {
            insert = "INSERT OR REPLACE";
        }
        else
        {
            insert = "REPLACE";
        }
        break;
    case SqlInsertStatementTypes::InsertOrIgnore:
        if(driver->dbmsType()==QSqlDriver::SQLite)
        {
            insert = "INSERT OR IGNORE";
        }
        else
        {
            insert = "INSERT IGNORE";
        }
        break;
    default:
        insert = "INSERT";
        break;
    }
    s.replace("INSERT", insert);
    s.remove(s.indexOf(" VALUES"), s.size()-s.indexOf(" VALUES"));

    return s;
}
QString Sql::valuesInsertStatement(const QString &tableName, const QVariantMap &map, const QSqlDriver *driver)
{
    if(!driver)
        driver = SqlDbPool::defaultDriver();

    if(!driver)
    {
        SQLLOG_WARNING()<<"No driver has been given";
        return QString();
    }

    const QSqlRecord rec = driver->record(tableName);

    QStringList values;
    for(int i=0; i<rec.count(); i++)
        values.append(Sql::formatValue(map.value(rec.fieldName(i)), driver));

    return QString("(%1)").arg(values.join(','));
}
QString Sql::insertMultipleStatement(const QString &tableName, const QVariantList &maps, const QSqlDriver *driver, SqlInsertStatementTypes::Enum insertType)
{
    if(!driver)
        driver = SqlDbPool::defaultDriver();

    if(!driver)
    {
        SQLLOG_WARNING()<<"No driver has been given";
        return QString();
    }

    if(maps.isEmpty())
        return QString();

    QString s = Sql::emptyInsertStatement(tableName, driver, insertType);
    QStringList values;
    for(const QVariant& var: maps)
        values.append(Sql::valuesInsertStatement(tableName, var.toMap(), driver));
    s.append(" VALUES ");
    s.append(values.join(','));

    return s;
}
QString Sql::basicInsertStatement(const QString &tableName, const QVariantMap &map, const QSqlDriver *driver, SqlInsertStatementTypes::Enum insertType)
{
    if(!driver)
        driver = SqlDbPool::defaultDriver();

    if(!driver)
    {
        SQLLOG_WARNING()<<"No driver has been given";
        return QString();
    }

    if(map.isEmpty())
        return QString();

    QString s = Sql::emptyInsertStatement(tableName, driver, insertType);
    const QString val = Sql::valuesInsertStatement(tableName, map, driver);

    s.append(" VALUES ");
    s.append(val);

    return s;
}
QString Sql::selectStatement(const QString &tableName, const QSqlRecord &rec, const QSqlDriver *driver, bool preparedStatement)
{
    if(!driver)
        driver = SqlDbPool::defaultDriver();

    if(!driver)
    {
        SQLLOG_WARNING()<<"No driver has been given";
        return QString();
    }

    return driver->sqlStatement(QSqlDriver::SelectStatement, tableName, rec, preparedStatement);
}
QString Sql::insertStatement(const QString &tableName, const QVariantMap &map, const QSqlDriver *driver, bool preparedStatement)
{
    return sqlStatement(QSqlDriver::InsertStatement, tableName, map, driver, preparedStatement);
}
QString Sql::updateStatement(const QString &tableName, const QVariantMap &map, const QSqlDriver *driver, bool preparedStatement)
{
    return sqlStatement(QSqlDriver::UpdateStatement, tableName, map, driver, preparedStatement);
}
QString Sql::whereStatement(const QString &tableName, const QVariantMap &map, const QSqlDriver *driver, bool preparedStatement)
{
    QStringList filters;
    for(auto [key, value]: map.asKeyValueRange())
    {
        const QString filter = filterEquals(tableName, key, value, driver, preparedStatement);
        if(!filter.isEmpty())
            filters.append(filter);
    }

    QString s;
    s = filters.join(" AND ");

    return Sql::where(s);
}
QString Sql::deleteWhereStatement(const QString &tableName, const QVariantMap &map, const QSqlDriver *driver, bool preparedStatement)
{
    if(!driver)
        driver = SqlDbPool::defaultDriver();

    if(!driver)
    {
        SQLLOG_WARNING()<<"No driver has been given";
        return QString();
    }

    if(map.isEmpty())
        return QString();

    const QString s = sqlStatement(QSqlDriver::DeleteStatement, tableName, map, driver, preparedStatement);
    const QString where = whereStatement(tableName, map, driver, preparedStatement);

    if(where.isEmpty())
        return QString();

    return Sql::concat(s, where);
}
QString Sql::sqlStatement(QSqlDriver::StatementType type, const QString &tableName, const QVariantMap &map, const QSqlDriver *driver, bool preparedStatement)
{
    if(!driver)
        driver = SqlDbPool::defaultDriver();

    if(!driver)
    {
        SQLLOG_WARNING()<<"No driver has been given";
        return QString();
    }

    const QSqlRecord record = prepareRecord(tableName, map, driver);

    return sqlStatement(type, tableName, record, driver, preparedStatement);
}

QString Sql::emptyInsertStatement(const QString &tableName, const QSqlRecord &rec, const QSqlDriver *driver)
{
    if(!driver)
        driver = SqlDbPool::defaultDriver();

    if(!driver)
    {
        SQLLOG_WARNING()<<"No driver has been given";
        return QString();
    }

    QString s = driver->sqlStatement(QSqlDriver::InsertStatement, tableName, rec, true);
    s.remove(s.indexOf(" VALUES"), s.size()-s.indexOf(" VALUES"));

    return s;
}
QString Sql::insertValueStatement(const QString &tableName, const QSqlRecord &rec, const QSqlDriver *driver)
{
    if(!driver)
        driver = SqlDbPool::defaultDriver();

    if(!driver)
    {
        SQLLOG_WARNING()<<"No driver has been given";
        return QString();
    }

    QStringList values;
    for(int i=0; i<rec.count(); i++)
        values.append(Sql::formatValue(rec.field(i).value(), driver));

    return QString("(%1)").arg(values.join(','));
}
QString Sql::insertStatement(const QString &tableName, const QSqlRecord &rec, const QSqlDriver *driver, bool preparedStatement)
{
    return sqlStatement(QSqlDriver::InsertStatement, tableName, rec, driver, preparedStatement);
}
QString Sql::updateStatement(const QString &tableName, const QSqlRecord &rec, const QSqlDriver *driver, bool preparedStatement)
{
    return sqlStatement(QSqlDriver::UpdateStatement, tableName, rec, driver, preparedStatement);
}
QString Sql::sqlStatement(QSqlDriver::StatementType type, const QString &tableName, const QSqlRecord &rec, const QSqlDriver *driver, bool preparedStatement)
{
    if(!driver)
        driver = SqlDbPool::defaultDriver();

    if(!driver)
    {
        SQLLOG_WARNING()<<"No driver has been given";
        return QString();
    }

    return driver->sqlStatement(type, tableName, rec, preparedStatement);
}
