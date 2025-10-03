#ifndef RESTSUBMITWORKER_H
#define RESTSUBMITWORKER_H

#include "syncable/qspatch.h"
#include "syncable/qsworker.h"
#include "rest_helpertypes.h"

class RestClass;
class RestReply;
class RestSubmitWorker : public QSWorker
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, connection, Connection, RestHelper::defaultConnection)
    Q_WRITABLE_REF_PROPERTY(QString, path, Path, {})
    Q_WRITABLE_REF_PROPERTY(QString, method, Method, {})
    Q_WRITABLE_REF_PROPERTY(QVariantMap, parameters, Parameters, {})
    Q_WRITABLE_REF_PROPERTY(QVariantMap, headers, Headers, {})
    Q_WRITABLE_REF_PROPERTY(QString, primaryField, PrimaryField, "")
    Q_WRITABLE_REF_PROPERTY(QVariantList, source, Source, {})
    Q_WRITABLE_REF_PROPERTY(QVariantList, destination, Destination, {})

public:
    explicit RestSubmitWorker(QObject *parent = nullptr);

public slots:
    bool doRun() final override;
    bool abort() final override;
    bool waitForFinished(int timeout = -1, QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents) final override;

protected slots:
    void sendNextRequest();

protected:
    RestReply* patch(const QSPatch& patch);
    RestReply* insert(int index, const QVariant& variant);
    RestReply* set(int index, const QVariant& variant);
    RestReply* move(int from, int to, int count = 1);
    RestReply* remove(int index, int count = 1);

    QPointer<RestClass> m_class;
    QPointer<RestReply> m_reply;

    QSPatchSet m_patches;
    int m_patchIndex=0;
};

#endif // RESTSUBMITWORKER_H
