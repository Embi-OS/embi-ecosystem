#include "sqlquerygroupmodel.h"
#include "sql_log.h"

SqlQueryGroupModel::SqlQueryGroupModel(QObject *parent):
    SqlQueryModel(parent)
{
    connect(this, &SqlQueryGroupModel::tableNameChanged, this, &SqlQueryGroupModel::queueInvalidate);
    connect(this, &SqlQueryGroupModel::rawFilterChanged, this, &SqlQueryGroupModel::queueInvalidate);
    connect(this, &SqlQueryGroupModel::filterRoleNameChanged, this, &SqlQueryGroupModel::queueInvalidate);
    connect(this, &SqlQueryGroupModel::filterPatternChanged, this, &SqlQueryGroupModel::queueInvalidate);
    connect(this, &SqlQueryGroupModel::filterValueChanged, this, &SqlQueryGroupModel::queueInvalidate);
    connect(this, &SqlQueryGroupModel::filterInvertedChanged, this, &SqlQueryGroupModel::queueInvalidate);
    connect(this, &SqlQueryGroupModel::groupRoleNameChanged, this, &SqlQueryGroupModel::queueInvalidate);
    connect(this, &SqlQueryGroupModel::fieldsChanged, this, &SqlQueryGroupModel::queueInvalidate);
}

QString SqlQueryGroupModel::queryStatement()
{
    if (m_tableName.isEmpty()) {
        SQLLOG_WARNING()<<"No table name given";
        setLastError(QSqlError(QLatin1String("No table name given"), QString(),
                               QSqlError::StatementError));
        return QString();
    }

    QString final;

    final = selectClause();
    if(final.isEmpty())
        return QString();

    final = Sql::concat(final, filterClause());
    final = Sql::concat(final, groupByClause());

    SQLLOG_TRACE()<<final;

    return final;
}

QString SqlQueryGroupModel::selectClause()
{
    QSqlRecord baseRec = database().record(m_tableName);
    if (baseRec.isEmpty()) {
        SQLLOG_WARNING()<<"Unable to find table"<<m_tableName;
        setLastError(QSqlError(QLatin1String("Unable to find table %1").arg(m_tableName), QString(),
                               QSqlError::StatementError));
        return QString();
    }

    QSqlRecord rec;
    for (int i = 0; i < m_fields.count(); ++i)
    {
        QString columnName = m_fields.at(i);
        if(baseRec.contains(columnName))
            rec.append(baseRec.field(columnName));
    }

    if (rec.isEmpty())
        rec = baseRec;

    const QString stmt = driver()->sqlStatement(QSqlDriver::SelectStatement,
                                                m_tableName,
                                                rec,
                                                false);
    if (stmt.isEmpty()) {
        SQLLOG_WARNING()<<"Unable to select fields from table"<<m_tableName;
        setLastError(QSqlError(QLatin1String("Unable to select fields from table %1").arg(m_tableName),
                     QString(), QSqlError::StatementError));
        return stmt;
    }

    return stmt;
}

QString SqlQueryGroupModel::filterClause()
{
    QString filter;

    if(!getRawFilter().isEmpty())
    {
        filter = Sql::where(getRawFilter());
    }
    else if(!getFilterMap().isEmpty())
    {
        filter = Sql::whereStatement(getTableName(), getFilterMap(), driver());
    }
    else if(!getFilterRoleName().isEmpty() && !getFilterValue().isNull())
    {
        filter = Sql::whereStatement(getTableName(), {{getFilterRoleName(),getFilterValue()}}, driver());
    }
    else if(!getFilterRoleName().isEmpty() && !getFilterPattern().isEmpty())
    {
        filter = Sql::where(QString("%1 LIKE '%%2%'")
                                .arg(Sql::prepareIdentifier(getFilterRoleName(),QSqlDriver::FieldName,database().driver()),
                                     Sql::formatValue(getFilterPattern(), database().driver())));
    }
    filter.remove("WHERE ");

    if(getFilterInverted())
    {
        filter = Sql::inv(filter);
    }

    return Sql::where(filter);
}

QString SqlQueryGroupModel::groupByClause()
{
    return Sql::groupBy(getGroupRoleName());
}

