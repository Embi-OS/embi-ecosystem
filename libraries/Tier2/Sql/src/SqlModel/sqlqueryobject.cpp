#include "sqlqueryobject.h"
#include "sql_log.h"

SqlQueryObject::SqlQueryObject(QObject *parent):
    SqlQueryModel(parent)
{
    connect(this, &SqlQueryObject::tableNameChanged, this, &SqlQueryObject::onTableNameChanged);
    connect(this, &SqlQueryObject::tableNameChanged, this, &SqlQueryObject::queueInvalidate);
    connect(this, &SqlQueryObject::rawFilterChanged, this, &SqlQueryObject::queueInvalidate);
    connect(this, &SqlQueryObject::filterRoleNameChanged, this, &SqlQueryObject::queueInvalidate);
    connect(this, &SqlQueryObject::filterPatternChanged, this, &SqlQueryObject::queueInvalidate);
    connect(this, &SqlQueryObject::filterValueChanged, this, &SqlQueryObject::queueInvalidate);
    connect(this, &SqlQueryObject::filterInvertedChanged, this, &SqlQueryObject::queueInvalidate);
}

QSqlRecord SqlQueryObject::baseRecord() const
{
    return m_baseRec;
}

bool SqlQueryObject::isValid() const
{
    return SqlQueryModel::isValid() && rowCount()>=1;
}

bool SqlQueryObject::setValues(const QVariantMap& array)
{
    bool aRet=false;
    for(auto [key, value]: array.asKeyValueRange())
    {
        aRet = setValue(key, value);
    }

    return aRet;
}

bool SqlQueryObject::setValue(const QString& property, const QVariant& value)
{
    int roleIndex = this->columnIndex(property);

    return this->setData(this->index(0, 0), value, roleIndex);
}

QVariantMap SqlQueryObject::values(const QStringList& properties) const
{
    QHash<int,QByteArray> names = roleNames();
    if(!properties.isEmpty())
    {
        QHash<int,QByteArray> tmpNames=names;
        names.clear();
        for(const QString& role: properties)
        {
            const QByteArray roleName = role.toUtf8();
            names[tmpNames.key(roleName)]=roleName;
        }
    }

    QVariantMap map;
    QModelIndex modelIndex = index(0, 0);
    for (QHash<int, QByteArray>::iterator it = names.begin(); it != names.end(); ++it)
    {
        map.insert(it.value(), this->data(modelIndex, it.key()));
    }

    return map;
}

QVariant SqlQueryObject::value(const QString& property, const QVariant& defaultValue) const
{
    int roleIndex = this->columnIndex(property);

    QVariant ret = this->data(this->index(0, 0), roleIndex);
    if(!ret.isValid())
        ret = defaultValue;

    return ret;
}

QString SqlQueryObject::queryStatement()
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
    final = Sql::concat(final, Sql::limit(1));

    SQLLOG_TRACE()<<final;

    return final;
}

QString SqlQueryObject::selectClause()
{
    if (m_baseRec.isEmpty()) {
        SQLLOG_WARNING()<<"Unable to find table"<<m_tableName;
        setLastError(QSqlError(QLatin1String("Unable to find table %1").arg(m_tableName), QString(),
                               QSqlError::StatementError));
        return QString();
    }

    const QString stmt = driver()->sqlStatement(QSqlDriver::SelectStatement,
                                                m_tableName,
                                                m_baseRec,
                                                false);
    if (stmt.isEmpty()) {
        SQLLOG_WARNING()<<"Unable to select fields from table"<<m_tableName;
        setLastError(QSqlError(QLatin1String("Unable to select fields from table %1").arg(m_tableName),
                     QString(), QSqlError::StatementError));
        return stmt;
    }

    return stmt;
}

QString SqlQueryObject::filterClause()
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

void SqlQueryObject::onTableNameChanged(const QString& tableName)
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
