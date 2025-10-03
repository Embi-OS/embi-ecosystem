#ifndef RESTCLIENT_H
#define RESTCLIENT_H

#include <QDefs>
#include "rest_helpertypes.h"
#include "restrequestbuilder.h"
#include "restclass.h"

#include <QHostInfo>

class RestClient : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_VAR_PROPERTY(RestDataModes::Enum, dataMode, DataMode, RestDataModes::Json)
    Q_WRITABLE_REF_PROPERTY(QString, baseUrl, BaseUrl, {})
    Q_WRITABLE_VAR_PROPERTY(int, port, Port, -1)
    Q_WRITABLE_REF_PROPERTY(QVersionNumber, apiVersion, ApiVersion, {})
    Q_WRITABLE_VAR_PROPERTY(bool, trailingSlash, TrailingSlash, true)
    Q_WRITABLE_REF_PROPERTY(QVariantMap, globalHeaders, GlobalHeaders, {})
    Q_WRITABLE_REF_PROPERTY(QVariantMap, globalParameters, GlobalParameters, {})
    Q_WRITABLE_REF_PROPERTY(RestAttributeHash, requestAttributes, RequestAttributes, {})
    Q_WRITABLE_REF_PROPERTY(QString, username, Username, {})
    Q_WRITABLE_REF_PROPERTY(QString, password, Password, {})

    Q_WRITABLE_REF_PROPERTY(QString, name, Name, {})
    Q_COMPOSITION_PROPERTY(QNetworkAccessManager, manager, nullptr)
    Q_COMPOSITION_PROPERTY(RestClass, rootClass, nullptr)

    Q_WRITABLE_VAR_PROPERTY(bool, available, Available, true)

public:
    explicit RestClient(QObject *parent = nullptr);
    explicit RestClient(const QString& name, QObject *parent = nullptr);
    virtual ~RestClient();

    RestClass* createClass(const QString &path, QObject *parent = nullptr);

    virtual RestRequestBuilder builder() const;

public slots:
    void setModernAttributes();

    void addGlobalHeader(const QByteArray &name, const QByteArray &value);
    void removeGlobalHeader(const QByteArray &name);

    void addGlobalParameter(const QString &name, const QString &value);
    void removeGlobalParameter(const QString &name);

    void addRequestAttribute(QNetworkRequest::Attribute attribute, const QVariant &value);
    void removeRequestAttribute(QNetworkRequest::Attribute attribute);
};

#endif // RESTCLIENT_H
