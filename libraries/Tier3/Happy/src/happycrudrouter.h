#ifndef HAPPYCRUDROUTER_H
#define HAPPYCRUDROUTER_H

#include <Sql>

#include "happyrouter.h"
#include "fields/abstracthappyfield.h"

class HappyCrudRouter : public HappyRouter
{
    Q_OBJECT
    QML_ELEMENT

    Q_COMPOSITION_PROPERTY(SqlTablePreparator, sqlTablePreparator, nullptr)

    Q_WRITABLE_REF_PROPERTY(QString, connection, Connection, SqlDefaultConnection)
    Q_WRITABLE_REF_PROPERTY(QString, tableName, TableName, {})
    Q_WRITABLE_REF_PROPERTY(QString, lookupField, LookupField, {})
    Q_READONLY_REF_PROPERTY(QString, primaryField, PrimaryField, {})

    // Specify sql
    Q_WRITABLE_REF_PROPERTY(QList<SqlJoinQuery>, joins, Joins, {})
    Q_WRITABLE_REF_PROPERTY(QVariantList, defaultEntries, DefaultEntries, {})
    Q_WRITABLE_REF_PROPERTY(QVariantList, basicEntries, BasicEntries, {})

    Q_CONSTANT_OLP_PROPERTY(AbstractHappyField, fields)
    Q_DEFAULT_PROPERTY(fields)

public:
    explicit HappyCrudRouter(QObject *parent = nullptr);

    QVariantMap info() override;

    bool init(HappyServer* happyServer) override;

    Q_INVOKABLE AbstractHappyField* field(const QString& name) const;
    Q_INVOKABLE QVariant primaryFieldFromLookup(const QVariant& lookupValue) const;
    Q_INVOKABLE QVariant fieldFromLookup(const QString& field, const QVariant& lookupValue) const;
    Q_INVOKABLE QVariant fieldFromPrimary(const QString& field, const QVariant& primaryValue) const;

    QStringList parseColumns(const QStringList& fields, const QStringList& omit) const;
    QVariantMap parseFilters(const QVariantMap& filters) const;

    void preReadFields(const QSqlRecord& baseRec, const HappyHttpParameters& parameters=HappyHttpParameters(), const QStringList& columns=QStringList());
    void preWriteFields(const HappyHttpParameters& parameters=HappyHttpParameters());

    QVariant formatFieldRead(const QString& field, const QVariant& value, bool* ok=nullptr) const;
    QVariant formatFieldWrite(const QString& field, const QVariant& value, bool* ok=nullptr) const;

    QByteArray serializeFieldsJson(QSqlQuery&& query, const QStringList& columns=QStringList(), qsizetype* total=nullptr) const;
    QByteArray serializeFieldsCbor(QSqlQuery&& query, const QStringList& columns=QStringList(), qsizetype* total=nullptr) const;

    QVariantList readFields(QSqlQuery&& query, const QStringList& columns=QStringList(), qsizetype* total=nullptr) const;
    QVariantMap readFields(const QSqlRecord& record, const QStringList& columns=QStringList()) const;
    QVariantMap writeFields(const QVariantMap& object, bool creation) const;

    QVariantList getValues(const HappyHttpParameters& parameters=HappyHttpParameters());
    QVariantMap getValues(const QVariant& argValue, const HappyHttpParameters& parameters=HappyHttpParameters());

    virtual HappyReply getList(const HappyHttpParameters& parameters=HappyHttpParameters(), const HappyHttpHeaders& headers=HappyHttpHeaders());
    virtual HappyReply postObject(const QVariant& data, const HappyHttpParameters& parameters=HappyHttpParameters(), const HappyHttpHeaders& headers=HappyHttpHeaders());
    virtual HappyReply getObject(const QVariant& argValue, const HappyHttpParameters& parameters=HappyHttpParameters(), const HappyHttpHeaders& headers=HappyHttpHeaders());
    virtual HappyReply putObject(const QVariant& data, const QVariant& argValue, const HappyHttpParameters& parameters=HappyHttpParameters(), const HappyHttpHeaders& headers=HappyHttpHeaders());
    virtual HappyReply patchObject(const QVariant& data, const QVariant& argValue, const HappyHttpParameters& parameters=HappyHttpParameters(), const HappyHttpHeaders& headers=HappyHttpHeaders());
    virtual HappyReply deleteObject(const QVariant& argValue, const HappyHttpParameters& parameters=HappyHttpParameters(), const HappyHttpHeaders& headers=HappyHttpHeaders());

    QHttpServerResponse optionsRoute(const HappyHttpRequest &request);
    QHttpServerResponse getListRoute(const HappyHttpRequest &request);
    QHttpServerResponse postObjectRoute(const HappyHttpRequest &request);
    QHttpServerResponse getObjectRoute(const QVariant& argValue, const HappyHttpRequest &request);
    QHttpServerResponse putObjectRoute(const QVariant& argValue, const HappyHttpRequest &request);
    QHttpServerResponse patchObjectRoute(const QVariant& argValue, const HappyHttpRequest &request);
    QHttpServerResponse deleteObjectRoute(const QVariant& argValue, const HappyHttpRequest &request);

signals:
    void objectAboutToBeSaved(const QVariantMap& object);
    void objectSaved(const QVariant& primaryValue, const QVariantMap& object);
    void objectAboutToBeInserted(const QVariantMap& object);
    void objectInserted(const QVariant& primaryValue, const QVariantMap& object);
    void objectAboutToBeUpdated(const QVariant& primaryValue, const QVariantMap& object);
    void objectUpdated(const QVariant& primaryValue, const QVariantMap& object);
    void objectAboutToBeRemoved(const QVariant& primaryValue, const QVariantMap& object);
    void objectRemoved(const QVariant& primaryValue, const QVariantMap& object);

protected:
    virtual bool onObjectAboutToBeSaved(QVariantMap& object);
    virtual void onObjectSaved(const QVariant& primaryValue, const QVariantMap& object);
    virtual bool onObjectAboutToBeInserted(QVariantMap& object);
    virtual void onObjectInserted(const QVariant& primaryValue, const QVariantMap& object);
    virtual bool onObjectAboutToBeUpdated(const QVariant& primaryValue, QVariantMap& object);
    virtual void onObjectUpdated(const QVariant& primaryValue, const QVariantMap& object);
    virtual bool onObjectAboutToBeRemoved(const QVariant& primaryValue, const QVariantMap& object);
    virtual void onObjectRemoved(const QVariant& primaryValue, const QVariantMap& object);

    bool objectAboutToBeInsertedNotify(QVariantMap& object);
    void objectInsertedNotify(const QVariant& primaryValue, const QVariantMap& object);
    bool objectAboutToBeUpdatedNotify(const QVariant& primaryValue, QVariantMap& object);
    void objectUpdatedNotify(const QVariant& primaryValue, const QVariantMap& object);
    bool objectAboutToBeRemovedNotify(const QVariant& primaryValue, const QVariantMap& object);
    void objectRemovedNotify(const QVariant& primaryValue, const QVariantMap& object);

    QHash<QString, AbstractHappyField*> m_fieldsMap;
    AbstractHappyField* m_primaryHappyField=nullptr;
    AbstractHappyField* m_lookupHappyField=nullptr;
    QSqlRecord m_sqlRecord;
    QStringList m_sqlFields;
};

#endif // HAPPYCRUDROUTER_H
