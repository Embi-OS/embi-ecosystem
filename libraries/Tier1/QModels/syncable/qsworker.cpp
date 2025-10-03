#include "qsworker.h"

QSWorker::QSWorker(QObject *parent):
    QObject(parent)
{
    connect(this, &QSWorker::finished, this, &QSWorker::onWorkerFinished);
    connect(this, &QSWorker::succeeded, this, &QSWorker::onWorkerSucceeded);
    connect(this, &QSWorker::failed, this, &QSWorker::onWorkerFailed);
}

QSWorker* QSWorker::onFinished(FinishedFunction handler, Qt::ConnectionType connection)
{
    return onFinished(this, std::move(handler), connection);
}
QSWorker* QSWorker::onFinished(const QObject *scope, FinishedFunction handler, Qt::ConnectionType connection)
{
    if(!handler)
        return this;

    connect(this, &QSWorker::finished, scope, [xHandler = std::move(handler)](int status){
        xHandler(status);
    }, connection);

    return this;
}

QSWorker* QSWorker::onSucceeded(SucceededFunction handler, Qt::ConnectionType connection)
{
    return onSucceeded(this, std::move(handler), connection);
}
QSWorker* QSWorker::onSucceeded(const QObject *scope, SucceededFunction handler, Qt::ConnectionType connection)
{
    if(!handler)
        return this;

    connect(this, &QSWorker::succeeded, scope, [xHandler = std::move(handler)](int status, const QVariant &reply){
        xHandler(status, reply);
    }, connection);

    return this;
}
QSWorker* QSWorker::onFailed(FailedFunction handler, Qt::ConnectionType connection)
{
    return onFailed(this, std::move(handler), connection);
}
QSWorker* QSWorker::onFailed(const QObject *scope, FailedFunction handler, Qt::ConnectionType connection)
{
    if(!handler)
        return this;

    connect(this, &QSWorker::failed, scope, [xHandler = std::move(handler)](int status, const QString &error, const QVariant& reply){
        xHandler(status, error, reply);
    }, connection);

    return this;
}
QSWorker* QSWorker::onCanceled(CanceledFunction handler, Qt::ConnectionType connection)
{
    return onCanceled(this, std::move(handler), connection);
}
QSWorker* QSWorker::onCanceled(const QObject *scope, CanceledFunction handler, Qt::ConnectionType connection)
{
    if(!handler)
        return this;

    connect(this, &QSWorker::canceled, scope, [xHandler = std::move(handler)](){
        xHandler();
    }, connection);

    return this;
}

QSWorker *QSWorker::onProgress(ProgressFunction handler, Qt::ConnectionType connection)
{
    return onProgress(this, std::move(handler), connection);
}
QSWorker *QSWorker::onProgress(const QObject *scope, ProgressFunction handler, Qt::ConnectionType connection)
{
    if(!handler)
        return this;

    connect(this, &QSWorker::progress, scope, [xHandler = std::move(handler)](const QString& message, int progress){
        xHandler(message, progress);
    }, connection);

    return this;
}

QSWorker* QSWorker::disconnectAll(const QObject *scope)
{
    disconnect(this, &QSWorker::finished, scope, nullptr);
    disconnect(this, &QSWorker::succeeded, scope, nullptr);
    disconnect(this, &QSWorker::failed, scope, nullptr);
    disconnect(this, &QSWorker::canceled, scope, nullptr);
    disconnect(this, &QSWorker::progress, scope, nullptr);

    return this;
}

bool QSWorker::run()
{
    setRunning(true);
    bool result = doRun();

    if (!result)
        emit this->finished(-1);

    return result;
}

void QSWorker::onWorkerFinished(int status)
{
    setRunning(false);
    setStatus(status);

    if(m_autoDelete)
        this->deleteLater();
}

void QSWorker::onWorkerSucceeded(int status, const QVariant &reply)
{
    setRunning(false);
    setStatus(status);
    setReply(reply);
}

void QSWorker::onWorkerFailed(int status, const QString &error, const QVariant &reply)
{
    setRunning(false);
    setStatus(status);
    setError(error);
    setReply(reply);
}
