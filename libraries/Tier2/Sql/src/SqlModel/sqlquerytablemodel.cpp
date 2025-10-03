#include "sqlquerytablemodel.h"
#include "sql_log.h"

SqlQueryTableModel::SqlQueryTableModel(QObject *parent):
    SqlQueryModel(parent)
{
    connect(this, &SqlQueryTableModel::tableNameChanged, this, &SqlQueryTableModel::onTableNameChanged);
    connect(this, &SqlQueryTableModel::tableNameChanged, this, &SqlQueryTableModel::queueInvalidate);
    connect(this, &SqlQueryTableModel::rawFilterChanged, this, &SqlQueryTableModel::queueInvalidate);
    connect(this, &SqlQueryTableModel::filterRoleNameChanged, this, &SqlQueryTableModel::queueInvalidate);
    connect(this, &SqlQueryTableModel::filterPatternChanged, this, &SqlQueryTableModel::queueInvalidate);
    connect(this, &SqlQueryTableModel::filterValueChanged, this, &SqlQueryTableModel::queueInvalidate);
    connect(this, &SqlQueryTableModel::filterInvertedChanged, this, &SqlQueryTableModel::queueInvalidate);
    connect(this, &SqlQueryTableModel::limitChanged, this, &SqlQueryTableModel::queueInvalidate);
    connect(this, &SqlQueryTableModel::offsetChanged, this, &SqlQueryTableModel::queueInvalidate);
    connect(this, &SqlQueryTableModel::sortRoleNameChanged, this, &SqlQueryTableModel::queueInvalidate);
    connect(this, &SqlQueryTableModel::sortOrderChanged, this, &SqlQueryTableModel::queueInvalidate);
    connect(this, &SqlQueryTableModel::fieldsChanged, this, &SqlQueryTableModel::queueInvalidate);
}

QSqlRecord SqlQueryTableModel::baseRecord() const
{
    return m_baseRec;
}

QString SqlQueryTableModel::queryStatement()
{
    if (m_tableName.isEmpty()) {
        SQLLOG_WARNING()<<"No table name given";
        setLastError(QSqlError(QLatin1String("No table name given"), QString(),
                               QSqlError::StatementError));
        return QString();
    }

    QString final = selectClause();
    if(final.isEmpty())
        return QString();

    final = Sql::concat(final, filterClause());
    final = Sql::concat(final, orderByClause());
    final = Sql::concat(final, limitClause());
    final = Sql::concat(final, offsetClause());

    SQLLOG_TRACE()<<final;

    return final;
}

QString SqlQueryTableModel::selectClause()
{
    if (m_baseRec.isEmpty()) {
        SQLLOG_WARNING()<<"Unable to find table"<<m_tableName;
        setLastError(QSqlError(QLatin1String("Unable to find table %1").arg(m_tableName), QString(),
                               QSqlError::StatementError));
        return QString();
    }

    QSqlRecord rec;
    for (int i = 0; i < m_fields.count(); ++i)
    {
        QString columnName = m_fields.at(i);
        if(m_baseRec.contains(columnName))
            rec.append(m_baseRec.field(columnName));
    }

    if (rec.isEmpty())
        rec = m_baseRec;

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

QString SqlQueryTableModel::filterClause()
{
    QString filter;

    if(!getRawFilter().isEmpty())
    {
        filter = Sql::where(getRawFilter());
    }
    else if(!getFilterMap().isEmpty())
    {
        filter = Sql::whereStatement(getTableName(), getFilterMap(), database().driver());
    }
    else if(!getFilterRoleName().isEmpty() && !getFilterValue().isNull())
    {
        filter = Sql::whereStatement(getTableName(), {{getFilterRoleName(),getFilterValue()}}, database().driver());
    }
    else if(!getFilterRoleName().isEmpty() && !getFilterPattern().isEmpty())
    {
        filter = Sql::where(QString("%1 LIKE '%%2%'")
                .arg(Sql::prepareIdentifier(getFilterRoleName(),QSqlDriver::FieldName,driver()),
                     Sql::formatValue(getFilterPattern(), driver())));
    }
    filter.remove("WHERE ");

    if(getFilterInverted())
    {
        filter = Sql::inv(filter);
    }

    return Sql::where(filter);
}

QString SqlQueryTableModel::orderByClause()
{
    QString order;

    if(!getSortRoleName().isEmpty())
    {
        order=driver()->escapeIdentifier(getSortRoleName(), QSqlDriver::FieldName);
    }

    order = m_sortOrder == Qt::AscendingOrder ? Sql::asc(order) : Sql::desc(order);

    return Sql::orderBy(order);
}

QString SqlQueryTableModel::limitClause()
{
    return Sql::limit(getLimit());
}

QString SqlQueryTableModel::offsetClause()
{
    return Sql::offset(getOffset());
}

void SqlQueryTableModel::onTableNameChanged(const QString& tableName)
{
    m_baseRec = database().record(tableName);
    if (m_baseRec.isEmpty()) {
        SQLLOG_WARNING()<<"Unable to find table"<<m_tableName;
        setLastError(QSqlError(QLatin1String("Unable to find table %1").arg(m_tableName), QString(),
                               QSqlError::StatementError));
        return;
    }

    m_primaryIndex = database().primaryIndex(m_tableName);
    if(!m_primaryIndex.isEmpty())
        setPrimaryField(m_primaryIndex.fieldName(0));
}
