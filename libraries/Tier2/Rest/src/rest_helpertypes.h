#ifndef REST_HELPERTYPES_H
#define REST_HELPERTYPES_H

#include <QDefs>
#include <QUtils>
#include <QModels>

#include <QUrlQuery>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

#define Q_TRACE_RESTREPLY(reply) \
    reply->onFailed([](int httpStatus, const QVariant &reply) mutable { \
        qWarning()<<httpStatus; \
        qWarning().noquote()<<RestHelper::parseBody(reply); \
    }); \
    reply->onError(this, [](const QString &errorString, int code, RestReplyErrorTypes::Enum type, const QVariant &reply){ \
        qCritical().noquote()<<errorString; \
        qCritical()<<code; \
        qCritical()<<type; \
        qCritical().noquote()<<RestHelper::parseBody(reply); \
    }); \
    reply->onSucceeded(this, [](int status, const QVariant& reply) { \
        qTrace()<<status; \
        qTrace().noquote()<<RestHelper::parseBody(reply, false); \
    }); \
    reply->onFinished(this, [](int status){ \
        qTrace()<<status; \
    }); \

#define Q_TRACE_RESTREPLY_DURATION(reply) \
{ \
    QElapsedTimer* timer=new QElapsedTimer; \
    timer->start(); \
    reply->onFinished(this, [timer](int status){ \
        deferTrace()<<"RestReply took"<<timer->nsecsElapsed()/1000000.0<<"ms"; \
        delete timer; \
    }); \
} \

#define REST_DEFAULT_CONNECTION_TYPE Qt::AutoConnection

using RestAttributeHash = QHash<QNetworkRequest::Attribute, QVariant>;

Q_ENUM_CLASS(RestDataModes, RestDataMode,
             Json,
             Cbor)

Q_ENUM_CLASS(RestReplyErrorTypes, RestReplyErrorType,
             NoError,
             Unknown,
             //default error types
             Network,  // Indicates a network error, i.e. no internet
             Parser,  // Indicates that parsing the received JSON or CBOR data failed
             Failure,  // Indicates that the server sent a failure for the request

             //extended error types
             Deserialization  // Indicates that deserializing the received data to the target object failed. **Generic replies only!**
             )

Q_ENUM_CLASS(RestFilterOperators, RestFilterOperator,
             Equals,
             LessThan,
             LessEquals,
             GreaterThan,
             GreaterEquals,
             Range,
             In,
             Contains,
             StartsWith,
             EndsWith,
             RegExp,
             Date,
             Year,
             Month,
             Day,
             WeekDay,
             Hour,
             Minute,
             Second)

struct RestParserError
{
    int code = 0;
    QString error;

    void reset() {
        code=0;
        error.clear();
    }
};

class RestClient;
class RestClass;
class RestHelper : public QObject,
                   public QQmlSingleton<RestHelper>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_CONSTANT_PTR_PROPERTY(QStandardObjectModel, filterOperatorsModel, FilterOperatorsModel, nullptr)

protected:
    friend QQmlSingleton<RestHelper>;
    explicit RestHelper(QObject *parent = nullptr);

public:
    static const char * defaultConnection;

    static const QByteArray GetVerb;
    static const QByteArray PostVerb;
    static const QByteArray PutVerb;
    static const QByteArray DeleteVerb;
    static const QByteArray PatchVerb;
    static const QByteArray HeadVerb;
    static const QByteArray OptionsVerb;

    static const QByteArray ContentTypeEmpty;
    static const QByteArray ContentTypeDefault;
    static const QByteArray ContentTypeCbor;
    static const QByteArray ContentTypeJson;
    static const QByteArray ContentTypeText;
    static const QByteArray ContentTypeHtml;
    static const QByteArray ContentTypeUrlEncoded;
    static const QByteArray Accept;
    static const QByteArray ContentType;

    static QUrlQuery mapToQuery(const QVariantMap &map);
    static QNetworkReply *sendRequest(QNetworkAccessManager *manager,
                                      const QNetworkRequest &request,
                                      const QByteArray &verb,
                                      const QByteArray &body);

    Q_INVOKABLE static bool addGlobalApi(RestClient *client);
    Q_INVOKABLE static bool removeGlobalApi(const QString &name, bool deleteClient = true);
    Q_INVOKABLE static RestClient *defaultApiClient();
    Q_INVOKABLE static RestClient *apiClient(const QString &name);
    Q_INVOKABLE static RestClass *defaultApiRootClass();
    Q_INVOKABLE static RestClass *apiRootClass(const QString &name);
    Q_INVOKABLE static RestClass *createApiClass(const QString &name, const QString &path, QObject *parent = nullptr);

    Q_INVOKABLE static QString networkOperationToString(QNetworkAccessManager::Operation operation);
    Q_INVOKABLE static QString parseBody(const QVariant &data, bool compact=true);

    Q_INVOKABLE static QString formatFilterValue(const QVariant& value, bool inverted);
    Q_INVOKABLE static QString formatFilterName(const QString& field, RestFilterOperators::Enum op, bool inverted);
    Q_INVOKABLE static QString formatFieldName(const QString& field, const QString& type=QString());

    static QString dumpRequest(const QNetworkRequest &request,
                               const QByteArray &verb,
                               const QByteArray &body);

private:
    static QReadWriteLock globalApiLock;
    static QList<RestClient*> globalApis;
};

#endif // REST_HELPERTYPES_H
