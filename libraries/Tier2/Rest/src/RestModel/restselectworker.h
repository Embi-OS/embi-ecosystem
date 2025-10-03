#ifndef RESTSELECTWORKER_H
#define RESTSELECTWORKER_H

#include "syncable/qsworker.h"
#include "rest_helpertypes.h"

class RestClass;
class RestReply;
class RestSelectWorker : public QSWorker
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, connection, Connection, RestHelper::defaultConnection)
    Q_WRITABLE_REF_PROPERTY(QString, path, Path, {})
    Q_WRITABLE_REF_PROPERTY(QString, method, Method, {})
    Q_WRITABLE_REF_PROPERTY(QVariantMap, body, Body, {})
    Q_WRITABLE_REF_PROPERTY(QVariantMap, parameters, Parameters, {})
    Q_WRITABLE_REF_PROPERTY(QVariantMap, headers, Headers, {})
    Q_WRITABLE_VAR_PROPERTY(bool, autoParse, AutoParse, true)

public:
    explicit RestSelectWorker(QObject *parent = nullptr);

public slots:
    bool doRun() final override;
    bool abort() final override;
    bool waitForFinished(int timeout = -1, QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents) final override;

protected:
    QPointer<RestClass> m_class;
    QPointer<RestReply> m_reply;
};

#endif // RESTSELECTWORKER_H
