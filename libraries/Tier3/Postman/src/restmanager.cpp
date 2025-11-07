#include "restmanager.h"
#include "rest_log.h"

RestManager::RestManager(QObject *parent) :
    AbstractManager(parent)
{

}

bool RestManager::init()
{
    m_apiDataMode = RestDataModes::Json;

    // m_apiBaseUrl = "https://jsonplaceholder.typicode.com";
    // m_apiPort = 443;

    // m_apiBaseUrl = "https://10.6.36.104";
    // m_apiPort = 7443;

    // m_apiBaseUrl = "https://10.6.100.107";
    // m_apiPort = 7443;

    // m_apiBaseUrl = "http://10.6.100.107";
    // m_apiPort = 7080;

    // m_apiBaseUrl = "http://10.6.36.101";
    // m_apiPort = 32768;

    // m_apiBaseUrl = "http://voh-c-bte-0241";
    // m_apiPort = 7080;

    // m_apiBaseUrl = "https://192.168.0.100";
    // m_apiPort = 7443;

    // m_apiBaseUrl = "https://xlqqp19l-8000.euw.devtunnels.ms/";
    // m_apiPort = 443;

    // m_apiTrailingSlash = false;

    const QVariantMap args = AxionHelper::Get()->getArguments();
    if(args.contains("apiBaseUrl"))
        m_apiBaseUrl = args.value("apiBaseUrl").toString();
    if(args.contains("apiPort"))
        m_apiPort = args.value("apiPort").toInt();
    if(args.contains("apiKey"))
        m_apiKey = args.value("apiKey").toString();
    if(args.contains("apiDataMode"))
        m_apiDataMode = args.value("apiDataMode")=="Cbor" ? RestDataModes::Cbor : RestDataModes::Json;
    if(args.contains("apiTrailingSlash"))
        m_apiTrailingSlash = args.value("apiTrailingSlash").toBool();
    if(args.contains("apiNoRestSocket"))
        m_apiNoRestSocket = args.value("apiNoRestSocket").toBool();

    QSettingsMapper* persistantData = new QSettingsMapper(this);
    persistantData->setSelectPolicy(QVariantMapperPolicies::Manual);
    persistantData->setSubmitPolicy(QVariantMapperPolicies::Delayed);
    persistantData->setSettingsCategory(managerName());
    persistantData->select();
    persistantData->waitForSelect();

    persistantData->mapProperty(this,"apiDataMode");
    persistantData->mapProperty(this,"apiBaseUrl");
    persistantData->mapProperty(this,"apiPort");
    persistantData->mapProperty(this,"apiTrailingSlash");
    persistantData->mapProperty(this,"apiNoRestSocket");
    persistantData->mapProperty(this,"apiKey");

    RestSocket::setNoRestSocket(m_apiNoRestSocket);

    const QString hostName = QUrl(m_apiBaseUrl).host();
    const QHostAddress address = QHostAddress(hostName);
    m_isLocalhost = address.isLoopback() || hostName=="localhost";

    m_client = new RestClient(RestHelper::defaultConnection, this);
    m_client->setBaseUrl(m_apiBaseUrl);
    m_client->setPort(m_apiPort);
    m_client->setDataMode(m_apiDataMode);
    m_client->setTrailingSlash(m_apiTrailingSlash);

    if(!m_apiKey.isEmpty())
    {
        m_client->addGlobalHeader("Authorization", QString("Bearer %1").arg(m_apiKey).toUtf8());
        connect(this, &RestManager::apiKeyChanged, [this](const QString& apiKey){
            if(apiKey.isEmpty())
                m_client->removeGlobalHeader("Authorization");
            else
                m_client->addGlobalHeader("Authorization", QString("Bearer %1").arg(apiKey).toUtf8());
        });
    }

    return true;
}

bool RestManager::localhost() const
{
    return m_isLocalhost;
}

void RestManager::authenticate(const QString& identifier, const QString& password)
{
    if(identifier.isEmpty() && password.isEmpty())
    {
        resetApiKey();
        return;
    }

    QVariantMap auth;
    auth.insert("identifier", identifier);
    auth.insert("password", password);
    RestReply* reply = m_client->rootClass()->post("api/authentication/token", QVariant(auth));
    reply->onAllErrors(this, [](const QString &errorString, int error, RestReplyErrorTypes::Enum errorType, const QVariant& reply) {
        RESTLOG_WARNING()<<"Auth error:"<<errorString;
        QVariantMap settings;
        settings["title"] = tr("Erreur d'authentification");
        settings["caption"] = QString("Error: %1").arg(error);
        settings["details"] = errorString;
        settings["closable"] = true;
        SnackbarManager::Get()->showFatal(settings);
    });
    reply->onSucceeded(this, [this](int status, const QVariant& reply) {
        RESTLOG_INFO()<<"Auth token received:"<<status;
        const QVariantMap map = reply.toMap();
        const QString access = map.value("access").toString();
        const QString refresh = map.value("refresh").toString();

        QVariantMap settings;
        settings["title"] = tr("Authentification réussie");
        settings["details"] = access;
        SnackbarManager::Get()->showSuccess(settings);

        setApiKey(access);
    });
}
