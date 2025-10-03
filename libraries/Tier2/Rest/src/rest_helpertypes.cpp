#include "rest_helpertypes.h"
#include "rest_log.h"

#include "restclient.h"
#include "restclass.h"

const char * RestHelper::defaultConnection("defaultRestClient");

const QByteArray RestHelper::GetVerb("GET");
const QByteArray RestHelper::PostVerb("POST");
const QByteArray RestHelper::PutVerb("PUT");
const QByteArray RestHelper::DeleteVerb("DELETE");
const QByteArray RestHelper::PatchVerb("PATCH");
const QByteArray RestHelper::HeadVerb("HEAD");
const QByteArray RestHelper::OptionsVerb("OPTIONS");

const QByteArray RestHelper::ContentTypeEmpty = "application/x-empty";
const QByteArray RestHelper::ContentTypeDefault = "*/*";
const QByteArray RestHelper::ContentTypeCbor = "application/cbor";
const QByteArray RestHelper::ContentTypeJson = "application/json";
const QByteArray RestHelper::ContentTypeText = "text/plain";
const QByteArray RestHelper::ContentTypeHtml = "text/html";
const QByteArray RestHelper::ContentTypeUrlEncoded = "application/x-www-form-urlencoded";
const QByteArray RestHelper::Accept = "Accept";
const QByteArray RestHelper::ContentType = "Content-Type";

RestHelper::RestHelper(QObject *parent) :
    QObject(parent)
{
    QList<QStandardObject*> objects;
    objects.append(QStandardObject::valueObject(RestFilterOperators::Equals, formatFilterName("", RestFilterOperators::Equals, false)));
    objects.append(QStandardObject::valueObject(RestFilterOperators::LessThan, formatFilterName("", RestFilterOperators::LessThan, false)));
    objects.append(QStandardObject::valueObject(RestFilterOperators::LessEquals, formatFilterName("", RestFilterOperators::LessEquals, false)));
    objects.append(QStandardObject::valueObject(RestFilterOperators::GreaterThan, formatFilterName("", RestFilterOperators::GreaterThan, false)));
    objects.append(QStandardObject::valueObject(RestFilterOperators::GreaterEquals, formatFilterName("", RestFilterOperators::GreaterEquals, false)));
    objects.append(QStandardObject::valueObject(RestFilterOperators::Range, formatFilterName("", RestFilterOperators::Range, false)));
    objects.append(QStandardObject::valueObject(RestFilterOperators::Contains, formatFilterName("", RestFilterOperators::Contains, false)));
    objects.append(QStandardObject::valueObject(RestFilterOperators::StartsWith, formatFilterName("", RestFilterOperators::StartsWith, false)));
    objects.append(QStandardObject::valueObject(RestFilterOperators::EndsWith, formatFilterName("", RestFilterOperators::EndsWith, false)));
    objects.append(QStandardObject::valueObject(RestFilterOperators::RegExp, formatFilterName("", RestFilterOperators::RegExp, false)));
    objects.append(QStandardObject::valueObject(RestFilterOperators::Date, formatFilterName("", RestFilterOperators::Date, false)));
    objects.append(QStandardObject::valueObject(RestFilterOperators::Year, formatFilterName("", RestFilterOperators::Year, false)));
    objects.append(QStandardObject::valueObject(RestFilterOperators::Month, formatFilterName("", RestFilterOperators::Month, false)));
    objects.append(QStandardObject::valueObject(RestFilterOperators::Day, formatFilterName("", RestFilterOperators::Day, false)));
    objects.append(QStandardObject::valueObject(RestFilterOperators::WeekDay, formatFilterName("", RestFilterOperators::WeekDay, false)));
    objects.append(QStandardObject::valueObject(RestFilterOperators::Hour, formatFilterName("", RestFilterOperators::Hour, false)));
    objects.append(QStandardObject::valueObject(RestFilterOperators::Minute, formatFilterName("", RestFilterOperators::Minute, false)));
    objects.append(QStandardObject::valueObject(RestFilterOperators::Second, formatFilterName("", RestFilterOperators::Second, false)));
    m_filterOperatorsModel = QStandardObjectModel::valueModel(this);
    m_filterOperatorsModel->setObjects(objects);
}

QUrlQuery RestHelper::mapToQuery(const QVariantMap &map)
{
    QUrlQuery query;
    for (auto it = map.constBegin(); it != map.constEnd(); it++)
        query.addQueryItem(it.key(), it.value().toString());

    return query;
}

QReadWriteLock RestHelper::globalApiLock;
QList<RestClient*> RestHelper::globalApis;

bool restClientCompare(const RestClient* left, const QString& right) {
    return left->getName() < right;
}

QString RestHelper::dumpRequest(const QNetworkRequest &request, const QByteArray &verb, const QByteArray &body)
{
    QString string;
    string.append(QString("complete network query looks like:\n"));
    string.append(QString("%1\n").arg(qLogLine()));
    string.append(QString("    URL           : %1\n").arg(request.url().toString()));
    if(!request.url().userName().isEmpty())
        string.append(QString("    URL user      : %1\n").arg(request.url().userName()));
    if(!request.url().password().isEmpty())
        string.append(QString("    URL pass      : %1\n").arg(request.url().password()));
    if(!request.url().query().isEmpty())
        string.append(QString("    URL query     : %1\n").arg(request.url().query()));
    if(!verb.isEmpty())
        string.append(QString("    Verb          : %1\n").arg(verb));
    if(!body.isEmpty())
        string.append(QString("    Body          : %1\n").arg(body));

    const QList<QByteArray> rawHeaderList = request.rawHeaderList();
    if(!rawHeaderList.isEmpty()) {
        string.append(QString("    Headers       :\n"));
        for(const QByteArray& rawHeader: rawHeaderList)
            string.append(QString("         %1 %2\n").arg(QString::fromUtf8(rawHeader), QString::fromUtf8(request.rawHeader(rawHeader))));
    }

    string.append(QString("%1").arg(qLogLine()));
    return string;
}

QNetworkReply *RestHelper::sendRequest(QNetworkAccessManager *manager, const QNetworkRequest &request, const QByteArray &verb, const QByteArray &body)
{
    if(!manager)
    {
        RESTLOG_WARNING()<<"invalid manager";
        return nullptr;
    }

    RESTLOG_TRACE().noquote()<<RestHelper::dumpRequest(request, verb, body);
    // qNotice().noquote()<<RestHelper::dumpRequest(request, verb, body);

    QNetworkReply *reply = nullptr;
    if (body.isEmpty()) {
        reply = manager->sendCustomRequest(request, verb);
    }
    else {
        reply = manager->sendCustomRequest(request, verb, body);
    }
    return reply;
}

bool RestHelper::addGlobalApi(RestClient *client)
{
    if(!client)
        return false;

    QWriteLocker _{&RestHelper::globalApiLock};

    const QString name = client->getName();
    auto it = std::find_if(RestHelper::globalApis.begin(), RestHelper::globalApis.end(), [&](RestClient* client) {
        return client->getName()==name;
    });
    if (it != RestHelper::globalApis.end()) {
        RESTLOG_CRITICAL()<<"A RestClient with name"<<client->getName()<<"is already registered";
        return false;
    }

    if (RestHelper::globalApis.contains(client)) {
        RESTLOG_CRITICAL()<<"RestClient"<<client<<"is already registered";
        return false;
    }
    else {
        RESTLOG_INFO()<<"Add"<<client->getName()<<"Rest client";
        RestHelper::globalApis.append(client);
        return true;
    }
}

bool RestHelper::removeGlobalApi(const QString &name, bool deleteClient)
{
    RESTLOG_INFO()<<"Remove"<<name<<"Rest client";
    QWriteLocker _{&RestHelper::globalApiLock};

    int index=-1;
    auto it = std::find_if(RestHelper::globalApis.begin(), RestHelper::globalApis.end(), [&](RestClient* client) {
        return client->getName()==name;
    });
    if (it != RestHelper::globalApis.end()) {
        index = std::distance(RestHelper::globalApis.begin(), it);
    }

    if(index<0) {
        RESTLOG_WARNING()<<"Unable to remove"<<name<<"RestClient";
        return false;
    }

    auto client = RestHelper::globalApis.takeAt(index);

    if (deleteClient && client) {
        client->deleteLater();
    }

    return true;
}

RestClient *RestHelper::defaultApiClient()
{
    return apiClient(defaultConnection);
}

RestClient *RestHelper::apiClient(const QString &name)
{
    QReadLocker _{&RestHelper::globalApiLock};

    int index=-1;
    auto it = std::find_if(RestHelper::globalApis.begin(), RestHelper::globalApis.end(), [&](RestClient* client) {
        return client->getName()==name;
    });
    if (it != RestHelper::globalApis.end()) {
        index = std::distance(RestHelper::globalApis.begin(), it);
    }

    if(index<0) {
        RESTLOG_WARNING()<<"Unable to get"<<name<<"RestClient";
        return nullptr;
    }

    return RestHelper::globalApis.at(index);
}

RestClass *RestHelper::defaultApiRootClass()
{
    return apiRootClass(defaultConnection);
}

RestClass *RestHelper::apiRootClass(const QString &name)
{
    auto client = RestHelper::apiClient(name);
    if (client)
        return client->rootClass();
    else
        return nullptr;
}

RestClass *RestHelper::createApiClass(const QString &name, const QString &path, QObject *parent)
{
    auto client = RestHelper::apiClient(name);
    if (client)
        return client->createClass(path, parent);
    else
        return nullptr;
}

QString RestHelper::networkOperationToString(QNetworkAccessManager::Operation operation)
{
    switch (operation) {
    case QNetworkAccessManager::HeadOperation: return HeadVerb;
    case QNetworkAccessManager::GetOperation: return GetVerb;
    case QNetworkAccessManager::PutOperation: return PutVerb;
    case QNetworkAccessManager::PostOperation: return PostVerb;
    case QNetworkAccessManager::DeleteOperation: return DeleteVerb;
    case QNetworkAccessManager::CustomOperation: return "CUSTOM";
    default: return "Unknown operation";
    }
}

QString RestHelper::parseBody(const QVariant &data, bool compact)
{
    switch (data.userType()) {
    case QMetaType::QVariantMap:
    case QMetaType::QVariantList: {
        return QString::fromUtf8(QUtils::Log::variantToJson(data, compact));
        break;
    }
    default:
        if(data.userType() == qMetaTypeId<QJSValue>())
            return parseBody(data.value<QJSValue>().toVariant(), compact);
        return data.toString();
        break;
    }
}

QString RestHelper::formatFilterValue(const QVariant& value, bool inverted)
{
    if(value.metaType().id()!=QMetaType::QString && value.canConvert<QVariantList>())
    {
        const QVariantList values = value.toList();
        QStringList filterValues;
        for(const QVariant& val : values)
            filterValues.append(val.toString());

        if(filterValues.isEmpty())
            return QString();
        else if(filterValues.size()==1)
            return filterValues.at(0);
        else
            return filterValues.join(',');
    }
    else if(value.isNull())
    {
        if(inverted)
            return "False";
        else
            return "True";
    }

    return value.toString();
}

QString RestHelper::formatFilterName(const QString& field, RestFilterOperators::Enum op, bool inverted)
{
    QString filter = field;
    switch(op) {
    case RestFilterOperators::LessThan:
        filter.append("__lt");
        break;
    case RestFilterOperators::LessEquals:
        filter.append("__lte");
        break;
    case RestFilterOperators::GreaterThan:
        filter.append("__gt");
        break;
    case RestFilterOperators::GreaterEquals:
        filter.append("__gte");
        break;
    case RestFilterOperators::Range:
        filter.append("__range");
        break;
    case RestFilterOperators::In:
        filter.append("__in");
        break;
    case RestFilterOperators::Contains:
        filter.append("__icontains");
        break;
    case RestFilterOperators::StartsWith:
        filter.append("__istartswith");
        break;
    case RestFilterOperators::EndsWith:
        filter.append("__iendswith");
        break;
    case RestFilterOperators::RegExp:
        filter.append("__regex");
        break;
    case RestFilterOperators::Date:
        filter.append("__date");
        break;
    case RestFilterOperators::Year:
        filter.append("__year");
        break;
    case RestFilterOperators::Month:
        filter.append("__month");
        break;
    case RestFilterOperators::Day:
        filter.append("__day");
        break;
    case RestFilterOperators::WeekDay:
        filter.append("__weekday");
        break;
    case RestFilterOperators::Hour:
        filter.append("__hour");
        break;
    case RestFilterOperators::Minute:
        filter.append("__minute");
        break;
    case RestFilterOperators::Second:
        filter.append("__second");
        break;
    case RestFilterOperators::Equals:
    default:
        break;
    }

    if(inverted)
        filter.append("!");

    return filter;
}

QString RestHelper::formatFieldName(const QString& field, const QString& type)
{
    QString formatted = field.split('.', Qt::SkipEmptyParts).join("__");
    formatted.replace('[', "__idx_start__");
    formatted.replace(']', "__idx_end");
    if(!type.isEmpty())
        formatted.append(QString("__%1").arg(type.toLower()));
    return formatted;
}
