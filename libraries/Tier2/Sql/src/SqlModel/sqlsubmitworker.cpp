#include "sqlsubmitworker.h"
#include "sqlpatchable.h"

#include <QtConcurrentRun>
#include <QDeferred>

SqlSubmitWorker::SqlSubmitWorker(QObject *parent):
    QSWorker(parent)
{

}

bool SqlSubmitWorker::doRun()
{
    const QString connection = m_connection;
    const QString tableName = m_tableName;
    const QString primaryField = m_primaryField;
    const QStringList fields = m_fields;
    const QStringList omit = m_omit;
    const QStringList expand = m_expand;
    const QList<SqlJoinQuery> joins = m_joins;
    const QVariantList destination = m_destination;
    const QVariantList source = m_source;

    QDeferred<long long, QSqlError, QVariant> defer;
    QFuture<bool> future = QtConcurrent::run([defer, connection, tableName, primaryField, fields, omit, expand, joins, destination, source]() mutable {

        SqlPatchable sqlPatchable;
        sqlPatchable.setConnection(connection);
        sqlPatchable.setTableName(tableName);
        sqlPatchable.setPrimaryField(primaryField);
        sqlPatchable.setFields(fields);
        sqlPatchable.setOmit(omit);
        sqlPatchable.setExpand(expand);
        sqlPatchable.setJoins(joins);
        sqlPatchable.setSource(source);

        bool result = sqlPatchable.patch(destination);

        std::tuple<bool, long long, QSqlError, QVariant>(result, 0, sqlPatchable.lastError(), sqlPatchable.getSource());
        defer.end(result, 0, sqlPatchable.lastError(), sqlPatchable.getSource());
        return result;
    });

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
    }, Qt::QueuedConnection);

    return true;
}

bool SqlSubmitWorker::abort()
{
    return waitForFinished();
}

bool SqlSubmitWorker::waitForFinished(int timeout, QEventLoop::ProcessEventsFlags flags)
{
    if(!m_running)
        return true;

    QEventLoop loop;
    connect(this, &QSWorker::finished, &loop, &QEventLoop::exit, Qt::QueuedConnection);
    int status = loop.exec(flags);

    return status==QSqlError::NoError;
}
