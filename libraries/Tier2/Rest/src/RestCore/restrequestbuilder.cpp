#include "restrequestbuilder.h"
#include "rest_log.h"

RestRequestBuilder::RestRequestBuilder() = default;

RestRequestBuilder::RestRequestBuilder(const QUrl &baseUrl, QNetworkAccessManager *manager) :
    m_manager{manager},
    m_base{baseUrl},
    m_user{baseUrl.userName()},
    m_pass{baseUrl.password()},
    m_query{baseUrl.query()},
    m_fragment{baseUrl.fragment()},
    m_dataMode{RestDataModes::Json},
    m_verb{RestHelper::GetVerb},
    m_timeout{0}
{

}

RestRequestBuilder::RestRequestBuilder(const RestRequestBuilder &other) = default;

RestRequestBuilder::RestRequestBuilder(RestRequestBuilder &&other) noexcept = default;

RestRequestBuilder &RestRequestBuilder::operator=(const RestRequestBuilder &other) = default;

RestRequestBuilder &RestRequestBuilder::operator=(RestRequestBuilder &&other) noexcept = default;

RestRequestBuilder::~RestRequestBuilder() = default;

RestRequestBuilder &RestRequestBuilder::setNetworkAccessManager(QNetworkAccessManager *manager)
{
    m_manager = manager;
    return *this;
}

RestRequestBuilder &RestRequestBuilder::setExtender(RestRequestBuilderExtender* extender)
{
    m_extender = extender;
    return *this;
}

RestRequestBuilder &RestRequestBuilder::setCredentials(const QString& user, const QString& password)
{
    m_user = user;
    m_pass = password;
    return *this;
}

RestRequestBuilder &RestRequestBuilder::setVersion(const QVersionNumber& version)
{
    m_version = version;
    return *this;
}

RestRequestBuilder &RestRequestBuilder::setScheme(const QString& scheme)
{
    m_scheme = scheme;
    return *this;
}

RestRequestBuilder &RestRequestBuilder::addHeader(const QString &name, const QVariant &value)
{
    if(m_headers.contains(name)) {
        RESTLOG_WARNING()<<"header already contains"<<name<<"so it will be overriden";
    }

    m_headers.insert(name, value);
    return *this;
}

RestRequestBuilder &RestRequestBuilder::addHeaders(const QVariantMap &headers)
{
    for (auto it = headers.constBegin(); it != headers.constEnd(); it++)
        addHeader(it.key(), it.value());
    return *this;
}

RestRequestBuilder &RestRequestBuilder::updateFromRelativeUrl(const QUrl &url, bool mergeQuery, bool keepFragment)
{
    QUrl cUrl = buildUrl();

    m_base = resolveUrl(cUrl, url);

    if (m_base.host() != cUrl.host()) {
        RESTLOG_WARNING()<<"URL host changed from"
                                <<cUrl.host()
                                <<"to"
                                <<m_base.host();
    }
    //clear all the rest
    m_version = QVersionNumber();
    m_user.clear();
    m_pass.clear();
    m_path.clear();
    if (mergeQuery) {
        QUrlQuery query(url.query());
        for(const auto &item : query.queryItems(QUrl::FullyDecoded)) // clazy:exclude=range-loop
            m_query.addQueryItem(item.first, item.second);
    } else
        m_query = QUrlQuery(url.query());
    if (!keepFragment)
        m_fragment.clear();

    return *this;
}

RestRequestBuilder &RestRequestBuilder::addParameter(const QString &name, const QString &value)
{
    m_query.addQueryItem(name, value);
    return *this;
}

RestRequestBuilder &RestRequestBuilder::addParameters(const QVariantMap &parameters)
{
    for (auto [key, value] : parameters.asKeyValueRange())
        m_query.addQueryItem(key, value.toString());
    return *this;
}

RestRequestBuilder &RestRequestBuilder::addParameters(const QUrlQuery &parameters)
{
    for (const auto &param : parameters.queryItems(QUrl::FullyDecoded)) // clazy:exclude=range-loop
        m_query.addQueryItem(param.first, param.second);
    return *this;
}

RestRequestBuilder &RestRequestBuilder::setFragment(const QString& fragment)
{
    m_fragment = fragment;
    return *this;
}

RestRequestBuilder &RestRequestBuilder::addPath(const QString &pathSegment)
{
    m_path.append(pathSegment.split(QLatin1Char('/'), Qt::SkipEmptyParts));
    return *this;
}

RestRequestBuilder &RestRequestBuilder::addPath(const QStringList &pathSegment)
{
    m_path.append(pathSegment);
    return *this;
}

RestRequestBuilder &RestRequestBuilder::setTrailingSlash(bool enable)
{
    m_trailingSlash = enable;
    return *this;
}

RestRequestBuilder &RestRequestBuilder::setAttribute(QNetworkRequest::Attribute attribute, const QVariant &value)
{
    m_attributes.insert(attribute, value);
    return *this;
}

RestRequestBuilder &RestRequestBuilder::setAttributes(const RestAttributeHash &attributes)
{
    for (auto it = attributes.constBegin(); it != attributes.constEnd(); it++)
        m_attributes.insert(it.key(), it.value());
    return *this;
}

RestRequestBuilder &RestRequestBuilder::setBody(const QVariant &body)
{
    if(body.userType() == qMetaTypeId<QJSValue>())
        m_body = body.value<QJSValue>().toVariant();
    else
        m_body = body;
    m_postQuery.clear();
    return *this;
}

RestRequestBuilder &RestRequestBuilder::setVerb(const QByteArray& verb)
{
    m_verb = verb;
    return *this;
}

RestRequestBuilder &RestRequestBuilder::setDataMode(RestDataModes::Enum dataMode)
{
    m_dataMode = dataMode;
    return *this;
}

RestRequestBuilder &RestRequestBuilder::setTimeout(const int timeout)
{
    m_timeout = timeout;
    return *this;
}

RestRequestBuilder &RestRequestBuilder::addPostParameter(const QString &name, const QString &value)
{
    m_postQuery.addQueryItem(name, value);
    m_body.clear();
    m_headers.insert(RestHelper::ContentType, RestHelper::ContentTypeUrlEncoded);
    return *this;
}

RestRequestBuilder &RestRequestBuilder::addPostParameters(const QUrlQuery &parameters)
{
    for (const auto &param : parameters.queryItems(QUrl::FullyDecoded)) // clazy:exclude=range-loop
        m_postQuery.addQueryItem(param.first, param.second);
    m_body.clear();
    m_headers.insert(RestHelper::ContentType, RestHelper::ContentTypeUrlEncoded);
    return *this;
}

QUrl RestRequestBuilder::buildUrl() const
{
    QUrl url = m_base;

    if(!m_scheme.isNull())
        url.setScheme(m_scheme);

    QStringList pathList = url.path().split(QLatin1Char('/'), Qt::SkipEmptyParts);
    if (!m_version.isNull())
        pathList.append(QLatin1Char('v') + m_version.normalized().toString());
    pathList.append(m_path);
    url.setPath(QLatin1Char('/') + pathList.join(QLatin1Char('/')) + ((m_trailingSlash && !pathList.isEmpty()) ? QStringLiteral("/") : QString()));

    if (!m_user.isNull())
        url.setUserName(m_user);
    if (!m_pass.isNull())
        url.setPassword(m_pass);
    if (!m_query.isEmpty())
        url.setQuery(m_query);
    if (!m_fragment.isNull())
        url.setFragment(m_fragment);

    if (m_extender)
        m_extender->extendUrl(url);

    RESTLOG_TRACE()<<"built URL as"<<url.toString(QUrl::PrettyDecoded | QUrl::RemoveUserInfo);

    return url;
}

QNetworkRequest RestRequestBuilder::buildRequest() const
{
    QNetworkRequest request{buildUrl()};
    QByteArray verb = m_verb;
    QByteArray body;
    prepareRequest(request, &body);
    if (m_extender)
        m_extender->extendRequest(request, verb, &body);

    return request;
}

QNetworkReply *RestRequestBuilder::send() const
{
    QNetworkRequest request{buildUrl()};
    QByteArray verb = m_verb;
    QByteArray body;
    prepareRequest(request, &body);

    if (m_extender)
        m_extender->extendRequest(request, verb, &body);

    return RestHelper::sendRequest(m_manager, request, verb, body);
}

RestReply *RestRequestBuilder::exec(QObject* parent) const
{
    return new RestReply(send(), parent);
}

QUrl RestRequestBuilder::resolveUrl(QUrl base, QUrl relative)
{
    // if(!base.path().endsWith('/'))
    // {
    //     QString basePath = base.path()+'/';
    //     base.setPath(basePath);
    // }

    // if(relative.path().startsWith('/'))
    // {
    //     QString relativePath = relative.path().remove(0,1);
    //     relative.setPath(relativePath);
    // }

    QUrl ret = base.resolved(relative);

    return ret;
}

void RestRequestBuilder::prepareRequest(QNetworkRequest &request, QByteArray *sBody) const
{
    // add headers etc.
    for (auto it = m_headers.constBegin(); it != m_headers.constEnd(); it++) {
        request.setRawHeader(it.key().toUtf8(), it.value().toString().toUtf8());
    }
    for (auto it = m_attributes.constBegin(); it != m_attributes.constEnd(); it++) {
        request.setAttribute(it.key(), it.value());
    }
    request.setTransferTimeout(m_timeout);

    RESTLOG_TRACE()<<"created request with headers"<<m_headers.keys()<<"and attributes"<<m_attributes.keys();

    if(!request.hasRawHeader(RestHelper::Accept)) {
        if (m_dataMode == RestDataModes::Json) {
            request.setRawHeader(RestHelper::Accept, RestHelper::ContentTypeJson);
        }
        else if (m_dataMode == RestDataModes::Cbor) {
            request.setRawHeader(RestHelper::Accept, RestHelper::ContentTypeCbor);
        }
    }

    QByteArray contentType = request.rawHeader(RestHelper::ContentType);
    if(!request.hasRawHeader(RestHelper::ContentType)) {
        if (m_dataMode == RestDataModes::Json) {
            contentType = RestHelper::ContentTypeJson;
        }
        else if (m_dataMode == RestDataModes::Cbor) {
            contentType = RestHelper::ContentTypeCbor;
        }
    }

    // create the body
    if (sBody) {
        if (m_body.isValid())
        {
            if (contentType==RestHelper::ContentTypeJson) {
                *sBody = QUtils::Json::variantToJson(m_body, true);
            }
            else if (contentType==RestHelper::ContentTypeCbor) {
                *sBody = QUtils::Cbor::variantToCbor(m_body, QCborValue::UseFloat16);
            }
            else {
                *sBody = m_body.toString().toUtf8();
            }
            request.setRawHeader(RestHelper::ContentType, contentType);
        }
        else if (m_headers.value(RestHelper::ContentType) == RestHelper::ContentTypeUrlEncoded && !m_postQuery.isEmpty())
        {
            *sBody = m_postQuery.query().toUtf8();
            request.setRawHeader(RestHelper::ContentType, RestHelper::ContentTypeUrlEncoded);
        }
    }
}
