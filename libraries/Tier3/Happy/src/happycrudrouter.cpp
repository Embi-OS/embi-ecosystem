#include "happycrudrouter.h"
#include "happyhttpparameters.h"
#include "happyhttpheaders.h"
#include "happyserver.h"
#include "varhappyfield.h"
#include "foreignhappyfield.h"
#include "happy_log.h"

HappyCrudRouter::HappyCrudRouter(QObject *parent) :
    HappyRouter(parent),
    m_sqlTablePreparator(new SqlTablePreparator(this)),
    m_fields(this)
{
    m_sqlTablePreparator->setName(m_tableName);
    m_sqlTablePreparator->setDefaultEntries(m_defaultEntries);
    m_sqlTablePreparator->setBasicEntries(m_basicEntries);
    connect(this, &HappyCrudRouter::tableNameChanged, m_sqlTablePreparator, &SqlTablePreparator::setName);
    connect(this, &HappyCrudRouter::defaultEntriesChanged, m_sqlTablePreparator, &SqlTablePreparator::setDefaultEntries);
    connect(this, &HappyCrudRouter::basicEntriesChanged, m_sqlTablePreparator, &SqlTablePreparator::setBasicEntries);

    m_fields.onInserted([this](int index, AbstractHappyField* field) {
        if(VarHappyField* varField = qobject_cast<VarHappyField*>(field))
            m_sqlTablePreparator->columns().append(varField->sqlColumnPreparator());
        if(ForeignHappyField* foreignField = qobject_cast<ForeignHappyField*>(field))
            m_sqlTablePreparator->indexes().append(new SqlIndexPreparator(QStringList()<<foreignField->getName()));
    });
    m_fields.onRemoved([this](int index, AbstractHappyField* field) {
        if(VarHappyField* varField = qobject_cast<VarHappyField*>(field))
            m_sqlTablePreparator->columns().remove(varField->sqlColumnPreparator());
    });
}

QVariantMap HappyCrudRouter::info()
{
    QVariantMap map = HappyRouter::info();

    map.insert("tableName", getTableName());
    map.insert("lookupField", getLookupField());
    map.insert("primaryField", getPrimaryField());

    QStringList methods;
    methods.append(RestHelper::GetVerb);
    methods.append(RestHelper::PutVerb);
    methods.append(RestHelper::PostVerb);
    methods.append(RestHelper::PatchVerb);
    methods.append(RestHelper::DeleteVerb);
    map.insert("methods", methods);

    return map;
}

bool HappyCrudRouter::init(HappyServer* happyServer)
{
    bool result = HappyRouter::init(happyServer);

    if(!m_happyServer)
        return false;

    m_connection = happyServer->getSqlConnection();

    if (m_tableName.isEmpty()) {
        HAPPYLOG_WARNING()<<"No table name given";
        return false;
    }

    if(m_path.isEmpty()) {
        m_path = m_tableName;
        m_path.replace('_', '/');
    }

    m_sqlRecord = SqlDbPool::database(m_connection, false).record(m_tableName);
    if (m_sqlRecord.isEmpty()) {
        HAPPYLOG_WARNING()<<"Unable to find table:"<<m_tableName;
        return false;
    }

    for(int i = 0; i < m_sqlRecord.count(); ++i)
        m_sqlFields.append(m_sqlRecord.fieldName(i));

    QSqlIndex primaryIndex = SqlDbPool::database(m_connection, false).primaryIndex(m_tableName);
    if(!primaryIndex.isEmpty()) {
        setPrimaryField(primaryIndex.fieldName(0));
    }

    if(m_lookupField==m_primaryField) {
        setLookupField(m_primaryField);
    }

    for(AbstractHappyField* field: m_fields) {
        field->init(this, happyServer);
        m_fieldsMap.insert(field->getName(), field);
    }

    m_primaryHappyField = field(m_primaryField);
    m_lookupHappyField = field(m_lookupField);

    QHttpServer* server = happyServer->httpServer();
    if(!server)
        return false;

    if(!m_route)
        return result;

    server->route(QString("/%1/").arg(m_path), QHttpServerRequest::Method::Get,
                  [this](const QHttpServerRequest &request) {
                      const HappyHttpRequest happyRequest(request);
                      emit this->requestReceived(happyRequest);
                      return getListRoute(happyRequest);
                  });

    server->route(QString("/%1/").arg(m_path), QHttpServerRequest::Method::Post,
                  [this](const QHttpServerRequest &request) {
                      const HappyHttpRequest happyRequest(request);
                      emit this->requestReceived(happyRequest);
                      return postObjectRoute(happyRequest);
                  });

    server->route(QString("/%1/").arg(m_path), QHttpServerRequest::Method::Options,
                  [this](const QHttpServerRequest &request) {
                      const HappyHttpRequest happyRequest(request);
                      emit this->requestReceived(happyRequest);
                      return optionsRoute(happyRequest);
                  });

    server->route(QString("/%1/<arg>/").arg(m_path), QHttpServerRequest::Method::Get,
                  [this](const QString& argValue, const QHttpServerRequest &request) {
                      const HappyHttpRequest happyRequest(request);
                      emit this->requestReceived(happyRequest);
                      return getObjectRoute(argValue, happyRequest);
                  });

    server->route(QString("/%1/<arg>/").arg(m_path), QHttpServerRequest::Method::Put,
                  [this](const QString& argValue, const QHttpServerRequest &request) {
                      const HappyHttpRequest happyRequest(request);
                      emit this->requestReceived(happyRequest);
                      return putObjectRoute(argValue, happyRequest);
                  });

    server->route(QString("/%1/<arg>/").arg(m_path), QHttpServerRequest::Method::Patch,
                  [this](const QString& argValue, const QHttpServerRequest &request) {
                      const HappyHttpRequest happyRequest(request);
                      emit this->requestReceived(happyRequest);
                      return patchObjectRoute(argValue, happyRequest);
                  });

    server->route(QString("/%1/<arg>/").arg(m_path), QHttpServerRequest::Method::Delete,
                  [this](const QString& argValue, const QHttpServerRequest &request) {
                      const HappyHttpRequest happyRequest(request);
                      emit this->requestReceived(happyRequest);
                      return deleteObjectRoute(argValue, happyRequest);
                  });

    server->route(QString("/%1/<arg>/").arg(m_path), QHttpServerRequest::Method::Options,
                  [this](const QString& argValue, const QHttpServerRequest &request) {
                      const HappyHttpRequest happyRequest(request);
                      emit this->requestReceived(happyRequest);
                      return optionsRoute(happyRequest);
                  });

    return result;
}

AbstractHappyField* HappyCrudRouter::field(const QString& name) const
{
    return m_fieldsMap.value(name, nullptr);
}

QVariant HappyCrudRouter::primaryFieldFromLookup(const QVariant& lookupValue) const
{
    return fieldFromLookup(m_primaryField, lookupValue);
}

QVariant HappyCrudRouter::fieldFromLookup(const QString& field, const QVariant& lookupValue) const
{
    QVariant formattedLookup = m_lookupHappyField->formatWrite(lookupValue);

    if(m_lookupField==field)
        return formattedLookup;

    QSqlQuery selectReply = SqlBuilder::select(QStringList(field)).from(m_tableName).where(m_lookupField, formattedLookup).limit(1).offset(0)
                                       .connection(m_connection).forwardOnly().trust().exec();

    if(selectReply.lastError().isValid())
        return QVariant();

    if(!selectReply.seek(0))
        return QVariant();

    return selectReply.value(field);
}

QVariant HappyCrudRouter::fieldFromPrimary(const QString& field, const QVariant& primaryValue) const
{
    QVariant formattedPrimary = m_primaryHappyField->formatWrite(primaryValue);

    if(m_primaryField==field)
        return formattedPrimary;

    QSqlQuery selectReply = SqlBuilder::select(QStringList(field)).from(m_tableName).where(m_primaryField, formattedPrimary).limit(1).offset(0)
                                       .connection(m_connection).forwardOnly().trust().exec();

    if(selectReply.lastError().isValid())
        return QVariant();

    if(!selectReply.seek(0))
        return QVariant();

    return selectReply.value(field);
}

QStringList HappyCrudRouter::parseColumns(const QStringList& fields, const QStringList& omit) const
{
    QStringList columns = fields;

    if(columns.isEmpty() || columns.contains("*"))
    {
        columns.clear();
        for(AbstractHappyField* field: m_fields)
            columns.append(field->getName());
    }

    for(const QString& field: omit)
    {
        if(columns.contains(field))
            columns.removeAll(field);
    }

    return columns;
}

QVariantMap HappyCrudRouter::parseFilters(const QVariantMap& filters) const
{
    QVariantMap sqlFilters;

    for(auto [key, value]: filters.asKeyValueRange()) {
        AbstractHappyField* field = HappyCrudRouter::field(key.split("__").first());
        sqlFilters.insert(key, field->formatValue(value));
    }

    return sqlFilters;
}

void HappyCrudRouter::preReadFields(const QSqlRecord& baseRec, const HappyHttpParameters& parameters, const QStringList& columns)
{
    for(AbstractHappyField* field: m_fields) {
        const QString name = field->getName();
        if(columns.isEmpty() || columns.contains(name)) {
            field->preRead(parameters, baseRec);
        }
    }
}

void HappyCrudRouter::preWriteFields(const HappyHttpParameters& parameters)
{
    for(AbstractHappyField* field: m_fields)
        field->preWrite(parameters);
}

QVariant HappyCrudRouter::formatFieldRead(const QString& field, const QVariant& value, bool* ok) const
{
    AbstractHappyField* happyField = this->field(field);

    if(!happyField) {
        if(ok)
            *ok = false;
        return value;
    }

    return happyField->formatRead(value, ok);
}

QVariant HappyCrudRouter::formatFieldWrite(const QString& field, const QVariant& value, bool* ok) const
{
    AbstractHappyField* happyField = this->field(field);

    if(!happyField) {
        if(ok)
            *ok = false;
        return value;
    }

    return happyField->formatWrite(value, ok);
}

#include <QJsonVariant>
QByteArray HappyCrudRouter::serializeFieldsJson(QSqlQuery&& query, const QStringList& columns, qsizetype* total) const
{
    QList<AbstractHappyField*> fields;
    fields.reserve(m_fields.size());
    for(AbstractHappyField* field: m_fields) {
        const QString name = field->getName();
        if(columns.isEmpty() || columns.contains(name)) {
            fields.append(field);
        }
    }

    QByteArray json;
    QJsonVariantWriter writer(&json);
    writer.start();
    writer.startArray();

    qsizetype size = 0;
    bool exists = query.seek(0);
    while(exists)
    {
        size++;
        writer.startMap();
        qsizetype i = 0;
        const QSqlRecord rec = query.record();
        for(AbstractHappyField* field: fields)
        {
            writer.writeRaw(field->nameJson());
            writer.writeNameSeparator();

            VarHappyField* varField = qobject_cast<VarHappyField*>(field);
            if(varField && varField->getType()==SqlColumnTypes::Json && varField->nameIndex()>=0)
            {
                const QByteArray rawJson = rec.value(varField->nameIndex()).toByteArray();
                if(rawJson.isEmpty())
                    writer.writeRaw("null");
                else
                    writer.writeRaw(rawJson);
            }
            else
            {
                writer.writeVariant(field->read(rec));
            }
            if (++i < fields.size())
                writer.writeValueSeparator();
        }
        writer.endMap();

        exists = query.next();

        if(exists)
            writer.writeValueSeparator();
    }

    writer.endArray();

    json.squeeze();
    if(total)
        *total = size;

    return json;
}
QByteArray HappyCrudRouter::serializeFieldsCbor(QSqlQuery&& query, const QStringList& columns, qsizetype* total) const
{
    QList<AbstractHappyField*> fields;
    fields.reserve(m_fields.size());
    for(AbstractHappyField* field: m_fields) {
        const QString name = field->getName();
        if(columns.isEmpty() || columns.contains(name)) {
            fields.append(field);
        }
    }

    QByteArray cbor;
    QCborVariantWriter writer(&cbor);
    writer.start();
    if(total && *total>0)
        writer.startArray(*total);
    else
        writer.startArray();

    qsizetype size = 0;
    bool exists = query.seek(0);
    while(exists)
    {
        size++;
        writer.startMap(fields.size());
        const QSqlRecord rec = query.record();
        for(AbstractHappyField* field: fields)
        {
            writer.writeString(field->nameCbor());
            writer.writeVariant(field->read(rec));
        }
        writer.endMap();

        exists = query.next();
    }

    writer.endArray();

    cbor.squeeze();
    if(total)
        *total = size;

    return cbor;
}
QVariantList HappyCrudRouter::readFields(QSqlQuery&& query, const QStringList& columns, qsizetype* total) const
{
    QList<AbstractHappyField*> fields;
    fields.reserve(m_fields.size());
    for(AbstractHappyField* field: m_fields) {
        const QString name = field->getName();
        if(columns.isEmpty() || columns.contains(name)) {
            fields.append(field);
        }
    }

    QVariantList list;
    if(total && *total>0)
        list.reserve(*total);

    bool exists = query.seek(0);
    while(exists)
    {
        const QSqlRecord rec = query.record();
        QVariantMap resultRow;
        for(AbstractHappyField* field: fields) {
            resultRow.insert(field->getName(), field->read(rec));
        }
        list.append(std::move(resultRow));
        exists = query.next();
    }

    list.squeeze();

    if(total)
        *total = list.size();

    return list;
}

QVariantMap HappyCrudRouter::readFields(const QSqlRecord& record, const QStringList& columns) const
{
    QVariantMap object;

    for(AbstractHappyField* field: m_fields) {
        QString name = field->getName();
        if(columns.isEmpty() || columns.contains(name)) {
            object.insert(std::move(name), field->read(record));
        }
    }

    return object;
}

QVariantMap HappyCrudRouter::writeFields(const QVariantMap& object, bool creation) const
{
    QVariantMap formattedMap;

    for(AbstractHappyField* field: m_fields) {
        bool ok=false;
        const QVariant value = field->write(object, creation, &ok);
        if(ok)
            formattedMap.insert(field->getName(), value);
    }

    return formattedMap;
}

QVariantList HappyCrudRouter::getValues(const HappyHttpParameters& parameters)
{
    const QStringList columns = parseColumns(parameters.fields, parameters.omit);

    const int limit = parameters.page<=0 ? parameters.limit : parameters.perPage;
    const int offset = parameters.page<=0 ? parameters.offset : (parameters.page-1)*parameters.perPage;
    const QVariantMap sqlFilters = parseFilters(parameters.filters);
    QSqlQuery sqlReply = SqlBuilder::select(columns).from(m_tableName).join(m_joins).where(sqlFilters).orderBy(parameters.sorters).limit(limit).offset(offset)
                                    .connection(m_connection).forwardOnly().exec();

    if(sqlReply.lastError().isValid())
    {
        return QVariantList();
    }

    const QSqlRecord record = sqlReply.record();

    preReadFields(record, parameters, columns);

    return readFields(std::move(sqlReply), columns);
}

QVariantMap HappyCrudRouter::getValues(const QVariant& argValue, const HappyHttpParameters& parameters)
{
    const QStringList columns = parseColumns(parameters.fields, parameters.omit);

    const QVariant lookupValue = m_lookupHappyField->formatWrite(argValue);

    const QVariantMap sqlFilters = parseFilters(parameters.filters);
    QSqlQuery sqlReply = SqlBuilder::select(columns).from(m_tableName).join(m_joins).where(m_lookupField, lookupValue).where(sqlFilters).limit(1).offset(0)
                                    .connection(m_connection).forwardOnly().exec();

    if(sqlReply.lastError().isValid())
    {
        return QVariantMap();
    }

    if(!sqlReply.seek(0))
    {
        return QVariantMap();
    }

    const QSqlRecord record = sqlReply.record();

    preReadFields(record, parameters, columns);

    return readFields(sqlReply.record(), columns);
}

HappyReply HappyCrudRouter::getList(const HappyHttpParameters& parameters, const HappyHttpHeaders& headers)
{
    QElapsedTimer timer;
    timer.start();
    const QStringList columns = parseColumns(parameters.fields, parameters.omit);

    const int limit = parameters.page<=0 ? parameters.limit : parameters.perPage;
    const int offset = parameters.page<=0 ? parameters.offset : (parameters.page-1)*parameters.perPage;
    const QVariantMap sqlFilters = parseFilters(parameters.filters);

    QSqlQuery selectReply = SqlBuilder::select(columns).from(m_tableName).join(m_joins).where(sqlFilters).orderBy(parameters.sorters).limit(limit).offset(offset)
                                       .connection(m_connection).forwardOnly().exec();

    if(selectReply.lastError().isValid())
    {
        return HappyReply::error(QString("%1 (%2)").arg(selectReply.lastError().text()).arg(selectReply.lastError().type()), QHttpServerResponder::StatusCode::InternalServerError);
    }

    const QSqlRecord record = selectReply.record();

    preReadFields(record, parameters, columns);

    qsizetype size = selectReply.size();

    if(parameters.flat && parameters.raw)
    {
        QByteArray contentType = RestHelper::ContentTypeJson;
        if(!headers.accept.isEmpty())
            contentType = headers.accept;
        else if(!headers.contentType.isEmpty())
            contentType = headers.contentType;

        QByteArray data;
        if (contentType == RestHelper::ContentTypeJson || contentType == RestHelper::ContentTypeDefault) {
            data = serializeFieldsJson(std::move(selectReply), columns, &size);
        }
        else if (contentType == RestHelper::ContentTypeCbor) {
            data = serializeFieldsCbor(std::move(selectReply), columns, &size);
        }

        return HappyReply(std::move(data));
    }

    QVariantList data = readFields(std::move(selectReply), columns, &size);

    if(parameters.flat)
        return HappyReply(std::move(data));

    long long total = size;
    long long page = 1;
    long long pageCount = 1;
    if((parameters.perPage>=0 && parameters.page>=0) ||
        (parameters.limit>=0 && parameters.offset>=0))
    {
        QSqlQuery reply = SqlBuilder::select("*").aggregate(SqlAggregateTypes::Count).from(m_tableName).where(sqlFilters)
                                     .connection(m_connection).forwardOnly().trust().exec();
        if(reply.seek(0))
        {
            total = reply.record().value(0).toLongLong();
        }

        if(parameters.perPage>=0 || parameters.page>=0)
        {
            page = parameters.page;
            pageCount = (parameters.perPage*total<=0) ? 1 : std::ceil((double)total/parameters.perPage);
        }
        else if(parameters.limit>=0 || parameters.offset>=0)
        {
            page = (parameters.limit<=0) ? 1 : (parameters.offset/parameters.limit) + 1;
            pageCount = (parameters.limit<=0) ? 1 : std::ceil((double)total/parameters.limit);
        }
    }

    QVariantMap map;
    map.insert("data", std::move(data));
    map.insert("page", page);
    map.insert("page_size", size);
    map.insert("page_count", pageCount);
    map.insert("total", total);

    return HappyReply(std::move(map));
}

HappyReply HappyCrudRouter::postObject(const QVariant& data, const HappyHttpParameters& parameters, const HappyHttpHeaders& headers)
{
    preWriteFields(parameters);

    QVariantMap object = writeFields(data.toMap(), true);

    objectAboutToBeInsertedNotify(object);

    QSqlQuery sqlReply = SqlBuilder::insert(object).into(m_tableName).connection(m_connection).exec();

    if(sqlReply.lastError().isValid())
    {
        return HappyReply::error(QString("%1 (%2)").arg(sqlReply.lastError().text()).arg(sqlReply.lastError().type()), QHttpServerResponder::StatusCode::InternalServerError);
    }

    const QVariant lastInsertedId = sqlReply.lastInsertId();
    objectInsertedNotify(lastInsertedId, object);

    const QStringList columns = parseColumns(parameters.fields, parameters.omit);

    QSqlQuery selectReply = SqlBuilder::select(columns).from(m_tableName).join(m_joins).where(m_primaryField, lastInsertedId).limit(1)
                                       .connection(m_connection).forwardOnly().exec();

    if(selectReply.lastError().isValid())
    {
        return HappyReply::error(QString("%1 (%2)").arg(selectReply.lastError().text()).arg(selectReply.lastError().type()), QHttpServerResponder::StatusCode::InternalServerError);
    }

    if(!selectReply.seek(0))
    {
        return HappyReply::error(QString("The new resource was not found on this server."), QHttpServerResponder::StatusCode::NotFound);
    }

    const QSqlRecord record = selectReply.record();

    preReadFields(record, parameters, columns);

    QVariantMap variant = readFields(selectReply.record(), columns);

    return HappyReply(std::move(variant), "CREATE");
}

HappyReply HappyCrudRouter::getObject(const QVariant& argValue, const HappyHttpParameters& parameters, const HappyHttpHeaders& headers)
{
    const QStringList columns = parseColumns(parameters.fields, parameters.omit);
    const QVariant lookupValue = m_lookupHappyField->formatWrite(argValue);

    QSqlQuery selectReply = SqlBuilder::select(columns).from(m_tableName).join(m_joins).where(m_lookupField, lookupValue).limit(1).offset(0)
                                       .connection(m_connection).forwardOnly().exec();

    if(selectReply.lastError().isValid())
    {
        return HappyReply::error(QString("%1 (%2)").arg(selectReply.lastError().text()).arg(selectReply.lastError().type()), QHttpServerResponder::StatusCode::InternalServerError);
    }

    if(!selectReply.seek(0))
    {
        return HappyReply::error(QString("The requested resource %1 was not found on this server.").arg(argValue.toString()), QHttpServerResponder::StatusCode::NotFound);
    }

    const QSqlRecord record = selectReply.record();

    preReadFields(record, parameters, columns);

    QVariantMap variant = readFields(selectReply.record(), columns);

    return HappyReply(std::move(variant));
}

HappyReply HappyCrudRouter::putObject(const QVariant& data, const QVariant& argValue, const HappyHttpParameters& parameters, const HappyHttpHeaders& headers)
{
    QVariant primaryValue = primaryFieldFromLookup(argValue);
    QVariantMap map = data.toMap();

    bool insert = false;
    if(primaryValue.isNull())
    {
        insert = true;
        map.insert(m_lookupField, argValue);
    }

    preWriteFields(parameters);

    QVariantMap object = writeFields(map, false);

    if(insert)
    {
        objectAboutToBeInsertedNotify(object);

        QSqlQuery sqlReply = SqlBuilder::insert(object).into(m_tableName).connection(m_connection).exec();

        if(sqlReply.lastError().isValid())
        {
            return HappyReply::error(QString("%1 (%2)").arg(sqlReply.lastError().text()).arg(sqlReply.lastError().type()), QHttpServerResponder::StatusCode::InternalServerError);
        }

        primaryValue = sqlReply.lastInsertId();
        objectInsertedNotify(primaryValue, object);
    }
    else
    {
        objectAboutToBeUpdatedNotify(primaryValue, object);

        QSqlQuery sqlReply = SqlBuilder::update(m_tableName).set(object).where(m_primaryField, primaryValue).connection(m_connection).exec();

        if(sqlReply.lastError().isValid())
        {
            return HappyReply::error(QString("%1 (%2)").arg(sqlReply.lastError().text()).arg(sqlReply.lastError().type()), QHttpServerResponder::StatusCode::InternalServerError);
        }

        objectUpdatedNotify(primaryValue, object);
    }

    const QStringList columns = parseColumns(parameters.fields, parameters.omit);

    QSqlQuery selectReply = SqlBuilder::select(columns).from(m_tableName).join(m_joins).where(m_primaryField, primaryValue).limit(1).offset(0)
                               .connection(m_connection).forwardOnly().exec();

    if(selectReply.lastError().isValid())
    {
        return HappyReply::error(QString("%1 (%2)").arg(selectReply.lastError().text()).arg(selectReply.lastError().type()), QHttpServerResponder::StatusCode::InternalServerError);
    }

    if(!selectReply.seek(0))
    {
        return HappyReply::error(QString("The requested resource %1 was not found on this server.").arg(argValue.toString()), QHttpServerResponder::StatusCode::NotFound);
    }

    const QSqlRecord record = selectReply.record();

    preReadFields(record, parameters, columns);

    QVariantMap variant = readFields(selectReply.record(), columns);

    return HappyReply(std::move(variant), insert?"CREATE":"UPDATE");
}

HappyReply HappyCrudRouter::patchObject(const QVariant& data, const QVariant& argValue, const HappyHttpParameters& parameters, const HappyHttpHeaders& headers)
{
    const QVariant primaryValue = primaryFieldFromLookup(argValue);

    if(primaryValue.isNull())
    {
        return HappyReply::error(QString("The requested resource %1 was not found on this server.").arg(argValue.toString()), QHttpServerResponder::StatusCode::NotFound);
    }

    preWriteFields(parameters);

    QVariantMap object = writeFields(data.toMap(), false);

    objectAboutToBeUpdatedNotify(primaryValue, object);

    QSqlQuery sqlReply = SqlBuilder::update(m_tableName).set(object).where(m_primaryField, primaryValue).connection(m_connection).exec();

    if(sqlReply.lastError().isValid())
    {
        return HappyReply::error(QString("%1 (%2)").arg(sqlReply.lastError().text()).arg(sqlReply.lastError().type()), QHttpServerResponder::StatusCode::InternalServerError);
    }

    objectUpdatedNotify(primaryValue, object);

    const QStringList columns = parseColumns(parameters.fields, parameters.omit);

    QSqlQuery selectReply = SqlBuilder::select(columns).from(m_tableName).join(m_joins).where(m_primaryField, primaryValue).limit(1).offset(0)
                                       .connection(m_connection).forwardOnly().exec();

    if(selectReply.lastError().isValid())
    {
        return HappyReply::error(QString("%1 (%2)").arg(selectReply.lastError().text()).arg(selectReply.lastError().type()), QHttpServerResponder::StatusCode::InternalServerError);
    }

    if(!selectReply.seek(0))
    {
        return HappyReply::error(QString("The requested resource %1 was not found on this server.").arg(argValue.toString()), QHttpServerResponder::StatusCode::NotFound);
    }

    const QSqlRecord record = selectReply.record();

    preReadFields(record, parameters, columns);

    QVariantMap variant = readFields(selectReply.record(), columns);

    return HappyReply(std::move(variant), "UPDATE");
}

HappyReply HappyCrudRouter::deleteObject(const QVariant& argValue, const HappyHttpParameters& parameters, const HappyHttpHeaders& headers)
{
    const QStringList columns = parseColumns(parameters.fields, parameters.omit);
    const QVariant lookupValue = m_lookupHappyField->formatWrite(argValue);

    QSqlQuery selectReply = SqlBuilder::select(columns).from(m_tableName).join(m_joins).where(m_lookupField, lookupValue).limit(1).offset(0)
                                       .connection(m_connection).forwardOnly().exec();

    if(selectReply.lastError().isValid())
    {
        return HappyReply::error(QString("%1 (%2)").arg(selectReply.lastError().text()).arg(selectReply.lastError().type()), QHttpServerResponder::StatusCode::InternalServerError);
    }

    if(!selectReply.seek(0))
    {
        return HappyReply::error(QString("The requested resource %1 was not found on this server.").arg(argValue.toString()), QHttpServerResponder::StatusCode::NotFound);
    }

    const QSqlRecord record = selectReply.record();

    preReadFields(record, parameters, columns);

    QVariantMap variant = readFields(selectReply.record(), columns);
    const QVariant primaryValue = variant.value(m_primaryField);

    objectAboutToBeRemovedNotify(primaryValue, variant);

    QSqlQuery sqlReply = SqlBuilder::delete_().from(m_tableName).where(m_primaryField, primaryValue).connection(m_connection).exec();

    if(sqlReply.lastError().isValid())
    {
        return HappyReply::error(QString("%1 (%2)").arg(sqlReply.lastError().text()).arg(sqlReply.lastError().type()), QHttpServerResponder::StatusCode::InternalServerError);
    }

    int numRowsAffected = sqlReply.numRowsAffected();
    if(numRowsAffected<=0)
    {
        return HappyReply::error(QString("The requested resource %1 was not found on this server.").arg(argValue.toString()), QHttpServerResponder::StatusCode::NotFound);
    }

    objectRemovedNotify(primaryValue, variant);

    return HappyReply(std::move(variant), "DELETE", QHttpServerResponse::StatusCode::NoContent);
}

QHttpServerResponse HappyCrudRouter::optionsRoute(const HappyHttpRequest &request)
{
    return HappyReply(info()).response(request.headers);
}

QHttpServerResponse HappyCrudRouter::getListRoute(const HappyHttpRequest &request)
{
    SqlDbPool::database(m_connection).transaction();
    const HappyReply reply = getList(request.parameters, request.headers);
    SqlDbPool::database(m_connection).commit();

    return reply.response(request.headers);
}

QHttpServerResponse HappyCrudRouter::postObjectRoute(const HappyHttpRequest &request)
{
    if(!request.parseOk)
    {
        return QHttpServerResponse(request.data.toString(), QHttpServerResponder::StatusCode::InternalServerError);
    }

    SqlDbPool::database(m_connection).transaction();
    const HappyReply reply = postObject(request.data, request.parameters, request.headers);
    SqlDbPool::database(m_connection).commit();

    return reply.response(request.headers);
}

QHttpServerResponse HappyCrudRouter::getObjectRoute(const QVariant& argValue, const HappyHttpRequest &request)
{
    SqlDbPool::database(m_connection).transaction();
    const HappyReply reply = getObject(argValue, request.parameters, request.headers);
    SqlDbPool::database(m_connection).commit();

    return reply.response(request.headers);
}

QHttpServerResponse HappyCrudRouter::putObjectRoute(const QVariant& argValue, const HappyHttpRequest &request)
{
    if(!request.parseOk)
    {
        return QHttpServerResponse(request.data.toString(), QHttpServerResponder::StatusCode::InternalServerError);
    }

    SqlDbPool::database(m_connection).transaction();
    const HappyReply reply = putObject(request.data, argValue, request.parameters, request.headers);
    SqlDbPool::database(m_connection).commit();

    return reply.response(request.headers);
}

QHttpServerResponse HappyCrudRouter::patchObjectRoute(const QVariant& argValue, const HappyHttpRequest &request)
{
    if(!request.parseOk)
    {
        return QHttpServerResponse(request.data.toString(), QHttpServerResponder::StatusCode::InternalServerError);
    }

    SqlDbPool::database(m_connection).transaction();
    const HappyReply reply = patchObject(request.data, argValue, request.parameters, request.headers);
    SqlDbPool::database(m_connection).commit();

    return reply.response(request.headers);
}

QHttpServerResponse HappyCrudRouter::deleteObjectRoute(const QVariant& argValue, const HappyHttpRequest &request)
{
    SqlDbPool::database(m_connection).transaction();
    const HappyReply reply = deleteObject(argValue, request.parameters, request.headers);
    SqlDbPool::database(m_connection).commit();

    return reply.response(request.headers);
}

bool HappyCrudRouter::onObjectAboutToBeSaved(QVariantMap& object)
{
    Q_UNUSED(object)
    return true;
}
void HappyCrudRouter::onObjectSaved(const QVariant& primaryValue, const QVariantMap& object)
{
    Q_UNUSED(primaryValue)
    Q_UNUSED(object)
}
bool HappyCrudRouter::onObjectAboutToBeInserted(QVariantMap& object)
{
    Q_UNUSED(object)
    return true;
}
void HappyCrudRouter::onObjectInserted(const QVariant& primaryValue, const QVariantMap& object)
{
    Q_UNUSED(primaryValue)
    Q_UNUSED(object)
}
bool HappyCrudRouter::onObjectAboutToBeUpdated(const QVariant& primaryValue, QVariantMap& object)
{
    Q_UNUSED(primaryValue)
    Q_UNUSED(object)
    return true;
}
void HappyCrudRouter::onObjectUpdated(const QVariant& primaryValue, const QVariantMap& object)
{
    Q_UNUSED(primaryValue)
    Q_UNUSED(object)
}
bool HappyCrudRouter::onObjectAboutToBeRemoved(const QVariant& primaryValue, const QVariantMap& object)
{
    Q_UNUSED(primaryValue)
    Q_UNUSED(object)
    return true;
}
void HappyCrudRouter::onObjectRemoved(const QVariant& primaryValue, const QVariantMap& object)
{
    Q_UNUSED(primaryValue)
    Q_UNUSED(object)
}

bool HappyCrudRouter::objectAboutToBeInsertedNotify(QVariantMap& object)
{
    bool ret = true;

    emit this->objectAboutToBeInserted(object);
    ret &= onObjectAboutToBeInserted(object);
    emit this->objectAboutToBeSaved(object);
    ret &= onObjectAboutToBeSaved(object);

    return ret;
}
void HappyCrudRouter::objectInsertedNotify(const QVariant& primaryValue, const QVariantMap& object)
{
    emit this->objectInserted(primaryValue, object);
    onObjectInserted(primaryValue, object);
    emit this->objectSaved(primaryValue, object);
    onObjectSaved(primaryValue, object);
}
bool HappyCrudRouter::objectAboutToBeUpdatedNotify(const QVariant& primaryValue, QVariantMap& object)
{
    bool ret = true;

    emit this->objectAboutToBeUpdated(primaryValue, object);
    ret &= onObjectAboutToBeUpdated(primaryValue, object);
    emit this->objectAboutToBeSaved(object);
    ret &= onObjectAboutToBeSaved(object);

    return ret;
}
void HappyCrudRouter::objectUpdatedNotify(const QVariant& primaryValue, const QVariantMap& object)
{
    emit this->objectUpdated(primaryValue, object);
    onObjectUpdated(primaryValue, object);
    emit this->objectSaved(primaryValue, object);
    onObjectSaved(primaryValue, object);
}
bool HappyCrudRouter::objectAboutToBeRemovedNotify(const QVariant& primaryValue, const QVariantMap& object)
{
    bool ret = true;

    emit this->objectAboutToBeRemoved(primaryValue, object);
    ret &= onObjectAboutToBeRemoved(primaryValue, object);

    return ret;
}
void HappyCrudRouter::objectRemovedNotify(const QVariant& primaryValue, const QVariantMap& object)
{
    emit this->objectRemoved(primaryValue, object);
    onObjectRemoved(primaryValue, object);
}
