#include "sqlselectworker.h"
#include "sqldbpool.h"
#include "sqlquerybuilder.h"

#include <QtConcurrentRun>
#include <QDeferred>

SqlSelectWorker::SqlSelectWorker(QObject *parent):
    QSWorker(parent)
{

}

bool SqlSelectWorker::doRun()
{
    const QString connection = m_connection;
    const QString tableName = m_tableName;
    const bool filterInverted = m_filterInverted;
    const QVariantMap filters = m_filters;
    const QVariantMap sorters = m_sorters;
    const QStringList fields = m_fields;
    const QStringList omit = m_omit;
    const QStringList expand = m_expand;
    const QList<SqlJoinQuery> joins = m_joins;
    const int limit = m_page<=0 ? m_limit : m_perPage;
    const int offset = m_page<=0 ? m_offset : (m_page-1)*m_perPage;
    const int perPage = m_perPage;
    const int page = m_page;

    QDeferred<long long, QSqlError, QVariant> defer;
    defer.progress([this](long long progress, const QSqlError&, const QVariant& reply) {
        emit this->progress(reply.toString(), progress);
    });
    defer.fail([this](long long, const QSqlError& error, const QVariant& reply) {
        emit this->failed(error.type(), error.text(), reply);
    });
    defer.done([this](long long, const QSqlError& error, const QVariant& reply) {
        emit this->succeeded(error.type(), reply);
    });
    defer.complete([this](bool, long long, const QSqlError& error, const QVariant&) {
        emit this->finished(error.type());
    }, m_asynchronous ? Qt::QueuedConnection : Qt::AutoConnection);

    auto run = [defer, connection, tableName, filterInverted, filters, sorters, fields, omit, expand, joins, limit, offset, perPage, page]() mutable {

        if (tableName.isEmpty()) {
            QSqlError error("No table name given", QString(), QSqlError::StatementError);
            defer.reject(0, error, QVariant());
            return false;
        }

        const QSqlRecord baseRec = SqlDbPool::database(connection).record(tableName);
        if (baseRec.isEmpty()) {
            QSqlError error(QString("Unable to find table: %1").arg(tableName), QString(), QSqlError::StatementError);
            defer.reject(0, error, QVariant());
            return false;
        }

        QStringList columns = fields;
        if(columns.isEmpty())
        {
            for (int i = 0; i < baseRec.count(); ++i)
                columns.append(baseRec.fieldName(i));
        }

        for(const QString& field: omit)
        {
            if(columns.contains(field))
                columns.removeAll(field);
        }

        QSqlQuery sqlReply = SqlBuilder::select(columns).from(tableName).join(joins).where(filters, filterInverted).orderBy(sorters).limit(limit).offset(offset)
                                        .connection(connection).forwardOnly().trust().exec();

        QVariantList data = SqlBuilder::values(sqlReply, perPage, page);
        qsizetype size = data.size();

        long long total = size;
        long long pageNb = 1;
        long long pageCount = 1;
        if((perPage>=0 && page>=0) ||
            (limit>=0 && offset>=0))
        {
            QSqlQuery reply = SqlBuilder::select("*").aggregate(SqlAggregateTypes::Count).from(tableName).where(filters, filterInverted)
                                         .connection(connection).forwardOnly().trust().exec();
            if(reply.seek(0))
            {
                total = reply.record().value(0).toLongLong();
            }

            if(perPage>=0 || page>=0)
            {
                pageNb = page;
                pageCount = (perPage*total<=0) ? 1 : std::ceil((double)total/perPage);
            }
            else if(limit>=0 || offset>=0)
            {
                pageNb = (limit<=0) ? 1 : (offset/limit) + 1;
                pageCount = (limit<=0) ? 1 : std::ceil((double)total/limit);
            }
        }

        QVariantMap map;
        map.insert("data", std::move(data));
        map.insert("page", pageNb);
        map.insert("page_size", size);
        map.insert("page_count", pageCount);
        map.insert("total", total);

        defer.end(!sqlReply.lastError().isValid(), 0, sqlReply.lastError(), map);
        return !sqlReply.lastError().isValid();
    };

    if(m_asynchronous)
        QFuture<bool> future = QtConcurrent::run(run);
    else
        run();

    return true;
}

bool SqlSelectWorker::abort()
{
    return waitForFinished();
}

bool SqlSelectWorker::waitForFinished(int timeout, QEventLoop::ProcessEventsFlags flags)
{
    if(!m_running)
        return true;

    QEventLoop loop;
    connect(this, &QSWorker::finished, &loop, &QEventLoop::exit, Qt::QueuedConnection);
    int status = loop.exec(flags);

    return status==QSqlError::NoError;
}
