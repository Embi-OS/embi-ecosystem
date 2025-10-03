#ifndef RESTREQUESTBUILDER_H
#define RESTREQUESTBUILDER_H

#include <QDefs>
#include "restreply.h"
#include "rest_helpertypes.h"

class RestRequestBuilderExtender
{
    Q_DISABLE_COPY(RestRequestBuilderExtender)

public:
    RestRequestBuilderExtender() = default;
    virtual ~RestRequestBuilderExtender() = default;

    virtual void extendUrl(QUrl &url) const { Q_UNUSED(url) };
    virtual bool requiresBody() const { return false; };
    virtual void extendRequest(QNetworkRequest& request, QByteArray& verb, QByteArray* body) const {Q_UNUSED(request); Q_UNUSED(verb); Q_UNUSED(body)};
};

class RestRequestBuilder
{
public:
    RestRequestBuilder();
    RestRequestBuilder(const QUrl &baseUrl, QNetworkAccessManager *manager = nullptr);
    RestRequestBuilder(const RestRequestBuilder &other);
    RestRequestBuilder(RestRequestBuilder &&other) noexcept;
    RestRequestBuilder &operator=(const RestRequestBuilder &other);
    RestRequestBuilder &operator=(RestRequestBuilder &&other) noexcept;
    ~RestRequestBuilder();

    RestRequestBuilder &setNetworkAccessManager(QNetworkAccessManager *manager);
    RestRequestBuilder &setExtender(RestRequestBuilderExtender *extender);

    RestRequestBuilder &setCredentials(const QString& user, const QString& password = {});
    RestRequestBuilder &setVersion(const QVersionNumber& version);
    RestRequestBuilder &setScheme(const QString& scheme);
    RestRequestBuilder &addPath(const QString& pathSegment);
    RestRequestBuilder &addPath(const QStringList& pathSegment);
    RestRequestBuilder &setTrailingSlash(bool enable = true);
    RestRequestBuilder &addParameter(const QString& name, const QString& value);
    RestRequestBuilder &addParameters(const QVariantMap& parameters);
    RestRequestBuilder &addParameters(const QUrlQuery& parameters);
    RestRequestBuilder &setFragment(const QString& fragment);
    RestRequestBuilder &addHeader(const QString& name, const QVariant& value);
    RestRequestBuilder &addHeaders(const QVariantMap& headers);

    RestRequestBuilder &updateFromRelativeUrl(const QUrl& url, bool mergeQuery=false, bool keepFragment=false);

    RestRequestBuilder &setAttribute(QNetworkRequest::Attribute attribute, const QVariant &value);
    RestRequestBuilder &setAttributes(const RestAttributeHash& attributes);

    RestRequestBuilder &setBody(const QVariant& body);
    RestRequestBuilder &setVerb(const QByteArray& verb);
    RestRequestBuilder &setDataMode(RestDataModes::Enum dataMode);
    RestRequestBuilder &setTimeout(const int timeout);

    RestRequestBuilder &addPostParameter(const QString& name, const QString& value);
    RestRequestBuilder &addPostParameters(const QUrlQuery& parameters);

    QUrl buildUrl() const;
    QNetworkRequest buildRequest() const;
    QNetworkReply *send() const;
    RestReply *exec(QObject* parent = nullptr) const;

    static QUrl resolveUrl(QUrl base, QUrl relative);

    const QUrl& base() const { return m_base; };
    const QVersionNumber& version() const { return m_version; };
    const QString& user() const { return m_user; };
    const QString& pass() const { return m_pass; };
    const QStringList& path() const { return m_path; };
    bool trailingSlash() const { return m_trailingSlash; };
    const QUrlQuery& query() const { return m_query; };
    const QString& fragment() const { return m_fragment; };
    const QVariantMap& headers() const { return m_headers; };
    const RestAttributeHash& attributes() const { return m_attributes; };
    const QVariant& body() const { return m_body; };
    const QByteArray& verb() const { return m_verb; };
    RestDataModes::Enum dataMode() const { return m_dataMode; };
    const QUrlQuery& postQuery() const { return m_postQuery; };

    void prepareRequest(QNetworkRequest& request, QByteArray* sBody=nullptr) const;

private:
    QNetworkAccessManager* m_manager=nullptr;
    RestRequestBuilderExtender* m_extender=nullptr;

    QUrl m_base;
    QVersionNumber m_version;
    QString m_scheme;
    QString m_user;
    QString m_pass;
    QStringList m_path;
    bool m_trailingSlash = false;
    QUrlQuery m_query;
    QString m_fragment;
    QVariantMap m_headers;
    RestAttributeHash m_attributes;
    QVariant m_body;
    RestDataModes::Enum m_dataMode;
    QByteArray m_verb;
    QUrlQuery m_postQuery;
    int m_timeout;
};

#endif // RESTREQUESTBUILDER_H
