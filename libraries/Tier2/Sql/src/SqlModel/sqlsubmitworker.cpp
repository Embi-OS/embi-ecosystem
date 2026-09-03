#include "sqlsubmitworker.h"
#include "sqlpatchable.h"

#include <QtConcurrentRun>
#include <QDeferred>
#include <QTimer>

SqlSubmitWorker::SqlSubmitWorker(QObject *parent):
    QSWorker(parent)
{

}

bool SqlSubmitWorker::doRun()
{
    m_abortRequested.store(false);
    const quint64 runId = ++m_runId;

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
    defer.progress([this, runId](long long progress, const QSqlError&, const QVariant& reply) {
        if(runId != m_runId.load() || m_abortRequested.load())
            return;
        emit this->progress(reply.toString(), progress);
    });
    defer.fail([this, runId](long long, const QSqlError& error, const QVariant& reply) {
        if(runId != m_runId.load() || m_abortRequested.load())
            return;
        emit this->failed(error.type(), error.text(), reply);
    });
    defer.done([this, runId](long long, const QSqlError& error, const QVariant& reply) {
        if(runId != m_runId.load() || m_abortRequested.load())
            return;
        emit this->succeeded(error.type(), reply);
    });
    defer.complete([this, runId](bool, long long, const QSqlError& error, const QVariant&) {
        if(runId != m_runId.load())
            return;
        if(m_abortRequested.load()) {
            emit this->canceled();
            emit this->finished(QSqlError::UnknownError);
            return;
        }
        emit this->finished(error.type());
    }, m_asynchronous ? Qt::QueuedConnection : Qt::AutoConnection);

    auto run = [defer, connection, tableName, primaryField, fields, omit, expand, joins, destination, source]() mutable {

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

        defer.end(result, 0, sqlPatchable.lastError(), sqlPatchable.getSource());
        return result;
    };

    if(m_asynchronous)
        QFuture<bool> future = QtConcurrent::run(run);
    else
        run();

    return true;
}

bool SqlSubmitWorker::abort()
{
    if(m_running)
        m_abortRequested.store(true);

    return true;
}

bool SqlSubmitWorker::waitForFinished(int timeout, QEventLoop::ProcessEventsFlags flags)
{
    if(!m_running)
        return m_status==QSqlError::NoError;

    QEventLoop loop;
    bool finished = false;
    int status = QSqlError::UnknownError;
    connect(this, &QSWorker::finished, &loop, [&loop, &finished, &status](int workerStatus) {
        finished = true;
        status = workerStatus;
        loop.quit();
    }, Qt::QueuedConnection);
    if(timeout>=0)
        QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
    loop.exec(flags);

    return finished && status==QSqlError::NoError;
}
