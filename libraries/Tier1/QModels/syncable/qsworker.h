#ifndef QSWORKER_H
#define QSWORKER_H

#include <QDefs>

#define QSWORKER_DEFAULT_CONNECTION_TYPE Qt::AutoConnection

class QSWorker : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(SyncableWorker)
    QML_UNCREATABLE("Abstract")

    Q_WRITABLE_VAR_PROPERTY(bool, autoDelete, AutoDelete, true)
    Q_READONLY_VAR_PROPERTY(bool, running, Running, false)

    Q_READONLY_REF_PROPERTY(int, status, Status, 0)
    Q_READONLY_REF_PROPERTY(QVariant, reply, Reply, {})
    Q_READONLY_REF_PROPERTY(QString, error, Error, {})

public:
    explicit QSWorker(QObject *parent = nullptr);

    typedef std::function<void(int)> FinishedFunction;
    typedef std::function<void(int, const QVariant&)> SucceededFunction;
    typedef std::function<void(int, const QString&, const QVariant&)> FailedFunction;
    typedef std::function<void()> CanceledFunction;
    typedef std::function<void(const QString&, int)> ProgressFunction;

    QSWorker* onFinished(FinishedFunction handler, Qt::ConnectionType connection = QSWORKER_DEFAULT_CONNECTION_TYPE);
    QSWorker* onFinished(const QObject *scope, FinishedFunction handler, Qt::ConnectionType connection = QSWORKER_DEFAULT_CONNECTION_TYPE);

    QSWorker* onSucceeded(SucceededFunction handler, Qt::ConnectionType connection = QSWORKER_DEFAULT_CONNECTION_TYPE);
    QSWorker* onSucceeded(const QObject *scope, SucceededFunction handler, Qt::ConnectionType connection = QSWORKER_DEFAULT_CONNECTION_TYPE);
    QSWorker* onFailed(FailedFunction handler, Qt::ConnectionType connection = QSWORKER_DEFAULT_CONNECTION_TYPE);
    QSWorker* onFailed(const QObject *scope, FailedFunction handler, Qt::ConnectionType connection = QSWORKER_DEFAULT_CONNECTION_TYPE);
    QSWorker* onCanceled(CanceledFunction handler, Qt::ConnectionType connection = QSWORKER_DEFAULT_CONNECTION_TYPE);
    QSWorker* onCanceled(const QObject *scope, CanceledFunction handler, Qt::ConnectionType connection = QSWORKER_DEFAULT_CONNECTION_TYPE);

    QSWorker* onProgress(ProgressFunction handler, Qt::ConnectionType connection = QSWORKER_DEFAULT_CONNECTION_TYPE);
    QSWorker* onProgress(const QObject *scope, ProgressFunction handler, Qt::ConnectionType connection = QSWORKER_DEFAULT_CONNECTION_TYPE);

    QSWorker* disconnectAll(const QObject *scope);

public slots:
    bool run();
    virtual bool doRun() = 0;
    virtual bool abort() = 0;
    virtual bool waitForFinished(int timeout = -1, QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents) = 0;

signals:
    void finished(int status);

    void succeeded(int status, const QVariant &reply);
    void failed(int status, const QString &error, const QVariant &reply);
    void canceled();

    void progress(const QString &message, int progress);

protected slots:
    void onWorkerFinished(int status);
    void onWorkerSucceeded(int status, const QVariant &reply);
    void onWorkerFailed(int status, const QString &error, const QVariant &reply);
};

#endif // QSWORKER_H
