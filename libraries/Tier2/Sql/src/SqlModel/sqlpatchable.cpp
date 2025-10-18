#include "sqlpatchable.h"
#include "sqldbpool.h"
#include "sqlquerybuilder.h"
#include "sql_log.h"

#include "syncable/qsdiffrunner.h"

SqlPatchable::SqlPatchable(QObject *parent):
    QObject(parent)
{

}

QSqlDatabase SqlPatchable::database() const
{
    return SqlDbPool::database(m_connection);
}

const QSqlDriver* SqlPatchable::driver() const
{
    return database().driver();
}

const QSqlError& SqlPatchable::lastError() const
{
    return m_lastError;
}

bool SqlPatchable::patch(const QVariantList& destination)
{
    if (m_tableName.isEmpty()) {
        QSqlError error("No table name given", QString(), QSqlError::StatementError);
        m_lastError = error;
        return false;
    }

    if(m_primaryField.isEmpty())
    {
        QSqlError error("No primary field given", QString(), QSqlError::StatementError);
        m_lastError = error;
        return false;
    }

    m_baseRec = database().record(m_tableName);
    if (m_baseRec.isEmpty()) {
        QSqlError error(QString("Unable to find table: %1").arg(m_tableName), QString(), QSqlError::StatementError);
        m_lastError = error;
        return false;
    }

    m_primaryIndex = database().primaryIndex(m_tableName);
    if(!m_primaryIndex.isEmpty() && m_primaryIndex.fieldName(0)!=m_primaryField) {
        SQLLOG_WARNING()<<"Primary field for table"<<m_tableName<<"differs from local primary filed"<<m_primaryIndex.fieldName(0)<<m_primaryField;
        setPrimaryField(m_primaryIndex.fieldName(0));
    }

    QElapsedTimer timer;
    timer.start();

    SqlDbPool::database(m_connection).transaction();

    const QSPatchSet patches = QSDiffRunner::compare(m_source, destination, m_primaryField);
    bool result = QSDiffRunner::patch(this, patches);

    if(result)
        SqlDbPool::database(m_connection).commit();
    else
        SqlDbPool::database(m_connection).rollback();

    SQLLOG_TRACE()<<"SqlPatchable::patch took:"<<timer.nsecsElapsed()/1000000.0;

    return result;
}

bool SqlPatchable::insert(int index, const QVariant& variant)
{
    QVariantList variants;
    if(variant.metaType().id()!=QMetaType::QString && variant.canConvert<QVariantList>())
        variants = variant.toList();
    else
        variants.append(variant);

    m_source.reserve(m_source.count() + variants.count());

    int offset = 0;
    for(const QVariant& var: variants)
    {
        QVariantMap map = var.toMap();
        QSqlQuery reply = SqlBuilder::insert(map).into(m_tableName).connection(m_connection).exec();
        m_lastError = reply.lastError();

        if(!m_lastError.isValid())
        {
            QSqlQuery query = SqlBuilder::select(m_columns).from(m_tableName).join(m_joins).where(m_primaryField, reply.lastInsertId()).limit(1).connection(m_connection).exec();
            if(!query.seek(0))
                return false;

            QVariantMap data;
            const QSqlRecord rec = query.record();
            for(int i = 0; i < rec.count(); ++i)
                data.insert(rec.fieldName(i), rec.value(i));

            for(auto [key, value]: data.asKeyValueRange()) {
                map.insert(key, value);
            }
            const int idx = index+offset;
            m_source.insert(idx, map);
            offset++;
        }
        else
            return false;
    }

    return true;
}

bool SqlPatchable::set(int index, const QVariant& variant)
{
    QVariantMap original = m_source.at(index).toMap();
    const QVariant primaryValue = original.value(m_primaryField);
    const QVariantMap map = variant.toMap();

    QSqlQuery reply = SqlBuilder::update(m_tableName).set(map).where(m_primaryField, primaryValue)
                                 .connection(m_connection).exec();
    m_lastError = reply.lastError();

    if(!m_lastError.isValid())
    {
        QSqlQuery query = SqlBuilder::select(m_columns).from(m_tableName).join(m_joins).where(m_primaryField, primaryValue).limit(1).connection(m_connection).exec();
        if(!query.seek(0))
            return false;

        QVariantMap data;
        const QSqlRecord rec = query.record();
        for(int i = 0; i < rec.count(); ++i)
            data.insert(rec.fieldName(i), rec.value(i));

        for (auto [key, value] : data.asKeyValueRange()) {
            original.insert(key, value);
        }
        m_source.replace(index, original);
    }

    return reply.result();
}

bool SqlPatchable::move(int from, int to, int count)
{
    for(int i = 0; i < count; ++i)
    {
        m_source.move(from, to);
    }

    SQLLOG_TRACE()<<"MOVE not supported";

    return true;
}

bool SqlPatchable::remove(int index, int count)
{
    QVariantList primaryValues;
    primaryValues.reserve(count);
    for(int i = 0; i < count; ++i) {
        const QVariantMap original = m_source.at(index+i).toMap();
        primaryValues << original.value(m_primaryField);
    }

    QSqlQuery reply = SqlBuilder::delete_().from(m_tableName).where(m_primaryField, primaryValues)
                                 .connection(m_connection).exec();
    m_lastError = reply.lastError();

    if(!m_lastError.isValid())
    {
        m_source.remove(index, count);
    }

    return reply.result();
}
