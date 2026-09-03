#include "restmanager.h"
#include "rest_log.h"

#ifdef Q_OS_WASM
#include <emscripten/val.h>
#endif

RestManager::RestManager(QObject *parent) :
    AbstractManager(parent)
{

}

bool RestManager::init()
{
    const auto endpointIsLoopback = [](const QString& baseUrl) {
        const QString trimmed = baseUrl.trimmed();
        const QUrl url = QUrl::fromUserInput(trimmed);
        const QString hostName = url.host();

        if(hostName.compare("localhost", Qt::CaseInsensitive)==0)
            return true;

        const QHostAddress address(hostName);
        return address.isLoopback();
    };

#ifdef Q_OS_WASM
    m_localApiEnabled = false;
    emscripten::val location = emscripten::val::global("location");
    m_apiBaseUrl = QString::fromStdString(location["hostname"].as<std::string>());
    const QString port = QString::fromStdString(location["port"].as<std::string>());
    m_apiPort = port.isEmpty() ? -1 : port.toInt();

    const QVariantMap args = AxionHelper::Get()->arguments();
    if(args.contains("apiBaseUrl"))
        m_apiBaseUrl = args.value("apiBaseUrl").toString();
    if(args.contains("apiPort"))
        m_apiPort = args.value("apiPort").toInt();
    if(args.contains("apiDataMode"))
        m_apiDataMode = args.value("apiDataMode")=="Cbor" ? RestDataModes::Cbor : RestDataModes::Json;
    if(args.contains("apiTrailingSlash"))
        m_apiTrailingSlash = args.value("apiTrailingSlash").toBool();
    if(args.contains("apiSocketEnabled"))
        m_apiSocketEnabled = args.value("apiSocketEnabled").toBool();
#else

    QSettingsMapper* persistantData = new QSettingsMapper(this);
    persistantData->setSelectPolicy(QVariantMapperPolicies::Manual);
    persistantData->setSubmitPolicy(QVariantMapperPolicies::Delayed);
    persistantData->setSettingsCategory(managerName());
    persistantData->select();
    persistantData->waitForSelect();

    const bool hasLocalApiEnabledSetting = persistantData->contains("localApiEnabled");

    persistantData->mapProperty(this,"localApiEnabled");
    persistantData->mapProperty(this,"apiDataMode");
    persistantData->mapProperty(this,"apiBaseUrl");
    persistantData->mapProperty(this,"apiPort");
    persistantData->mapProperty(this,"apiTrailingSlash");
    persistantData->mapProperty(this,"apiSocketEnabled");

    if(!hasLocalApiEnabledSetting)
        setLocalApiEnabled(endpointIsLoopback(m_apiBaseUrl));

#endif

    RestDataModes::Enum clientApiDataMode = m_apiDataMode;
    QString clientApiBaseUrl = m_apiBaseUrl;
    int clientApiPort = m_apiPort;
    bool clientApiTrailingSlash = m_apiTrailingSlash;
    bool clientApiSocketEnabled = m_apiSocketEnabled;

#ifndef Q_OS_WASM
    if(m_localApiEnabled)
    {
        clientApiDataMode = RestDataModes::Json;
        clientApiBaseUrl = QHostAddress(QHostAddress::LocalHost).toString();
        clientApiPort = 32768;
        clientApiTrailingSlash = true;
        clientApiSocketEnabled = true;
    }

    const QString trimmed = clientApiBaseUrl.trimmed();
    const QUrl url = QUrl::fromUserInput(trimmed);
    const QString hostName = url.host();
    const bool hostIsLocalhost = hostName.compare("localhost", Qt::CaseInsensitive)==0;

    if(hostIsLocalhost)
    {
        QUrl clientUrl = url;
        clientUrl.setHost(QHostAddress(QHostAddress::LocalHost).toString());
        clientApiBaseUrl = clientUrl.toString();
    }
#endif

    RestSocket::setGloballyEnabled(clientApiSocketEnabled);

    m_client = new RestClient(RestHelper::defaultConnection, this);
    m_client->setBaseUrl(clientApiBaseUrl);
    m_client->setPort(clientApiPort);
    m_client->setDataMode(clientApiDataMode);
    m_client->setTrailingSlash(clientApiTrailingSlash);

    RestRequestBuilder builder = m_client->builder();
    const QUrl effectiveUrl = builder.buildUrl();
    setApiEffectiveUrl(QString("%1 - %2").arg(effectiveUrl.toString(), RestDataModes::asString(m_client->getDataMode())));
    RESTLOG_INFO()<<"RestClient configuration:"<<m_apiEffectiveUrl;

    return true;
}
