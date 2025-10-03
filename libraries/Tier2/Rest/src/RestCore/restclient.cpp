#include "restclient.h"
#include "restclass.h"
#include "rest_log.h"

RestClient::RestClient(QObject *parent):
    RestClient("", parent)
{

}

RestClient::RestClient(const QString& name, QObject *parent):
    QObject(parent),
    m_name(name),
    m_manager(new QNetworkAccessManager(this)),
    m_rootClass(new RestClass("", this, this))
{
    connect(this, &RestClient::nameChanged, this, [this](){
        if(!m_name.isEmpty())
            RestHelper::addGlobalApi(this);
    }, Qt::SingleShotConnection);

    m_manager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);

    if(!m_name.isEmpty())
        RestHelper::addGlobalApi(this);
}

RestClient::~RestClient()
{
    if(!m_name.isEmpty())
        RestHelper::removeGlobalApi(m_name, false);
}

RestClass *RestClient::createClass(const QString &path, QObject *parent)
{
    RestClass* restClass = new RestClass(parent?parent:this);
    restClass->setClient(this);
    restClass->setPath(path);
    return restClass;
}

RestRequestBuilder RestClient::builder() const
{
    QUrl url = QUrl(m_baseUrl);
    if(m_port>=0)
        url.setPort(m_port);
    RestRequestBuilder builder{url, m_manager};

    builder.setVersion(m_apiVersion)
        .setAttributes(m_requestAttributes)
        .addHeaders(m_globalHeaders)
        .addParameters(m_globalParameters)
        .setCredentials(m_username, m_password)
        .setTrailingSlash(m_trailingSlash)
        .setDataMode(m_dataMode);

    return builder;
}

void RestClient::setModernAttributes()
{
    m_requestAttributes.insert(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
    m_requestAttributes.insert(QNetworkRequest::Http2AllowedAttribute, true);
    emit this->requestAttributesChanged(m_requestAttributes);
}

void RestClient::addGlobalHeader(const QByteArray &name, const QByteArray &value)
{
    m_globalHeaders.insert(name, value);
    emit this->globalHeadersChanged(m_globalHeaders);
}

void RestClient::removeGlobalHeader(const QByteArray &name)
{
    if(m_globalHeaders.remove(name) > 0)
        emit this->globalHeadersChanged(m_globalHeaders);
}

void RestClient::addGlobalParameter(const QString &name, const QString &value)
{
    m_globalParameters.insert(name, value);
    emit this->globalParametersChanged(m_globalParameters);
}

void RestClient::removeGlobalParameter(const QString &name)
{
    if(m_globalParameters.remove(name) > 0)
        emit this->globalParametersChanged(m_globalParameters);
}

void RestClient::addRequestAttribute(QNetworkRequest::Attribute attribute, const QVariant &value)
{
    m_requestAttributes.insert(attribute, value);
    emit this->requestAttributesChanged(m_requestAttributes);
}

void RestClient::removeRequestAttribute(QNetworkRequest::Attribute attribute)
{
    m_requestAttributes.remove(attribute);
    emit this->requestAttributesChanged(m_requestAttributes);
}
