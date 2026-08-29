#include "hassmanager.h"
#include "hass_log.h"

HassManager::HassManager(QObject *parent) :
    AbstractManager(parent)
{

}

bool HassManager::init()
{
    QSettingsMapper* persistantData = new QSettingsMapper(this);
    persistantData->setSelectPolicy(QVariantMapperPolicies::Manual);
    persistantData->setSubmitPolicy(QVariantMapperPolicies::Delayed);
    persistantData->setSettingsCategory(managerName());
    persistantData->select();
    persistantData->waitForSelect();

    persistantData->mapProperty(this,"apiBaseUrl");
    persistantData->mapProperty(this,"apiPort");
    persistantData->mapProperty(this,"apiToken");

    m_client = new RestClient("hass", this);
    m_client->setBaseUrl(m_apiBaseUrl);
    m_client->setPort(m_apiPort);
    m_client->setDataMode(RestDataModes::Json);
    m_client->setTrailingSlash(false);
    if(!m_apiToken.isEmpty())
        m_client->addGlobalHeader("Authorization", QString("Bearer %1").arg(m_apiToken).toUtf8());

    RestRequestBuilder builder = m_client->builder();
    const QUrl effectiveUrl = builder.buildUrl();
    setApiEffectiveUrl(QString("%1 - %2").arg(effectiveUrl.toString(), RestDataModes::asString(m_client->getDataMode())));
    HASSLOG_INFO()<<"RestClient configuration:"<<m_apiEffectiveUrl;

    connect(this, &HassManager::apiBaseUrlChanged, m_client, &RestClient::setBaseUrl);
    connect(this, &HassManager::apiPortChanged, m_client, &RestClient::setPort);
    connect(this, &HassManager::apiTokenChanged, [this](const QString& apiToken) {
        if(apiToken.isEmpty())
            m_client->removeGlobalHeader("Authorization");
        else
            m_client->addGlobalHeader("Authorization", QString("Bearer %1").arg(apiToken).toUtf8());
    });

    return true;
}

void HassManager::callService(const QString& domain,
                              const QString& service,
                              const QVariantMap& data)
{
    if(!m_client || domain.isEmpty() || service.isEmpty()) {
        SnackbarManager::Get()->showWarning(tr("Commande Home Assistant invalide"));
        return;
    }

    RestReply* reply = m_client->rootClass()->post(QString("api/services/%1/%2").arg(domain, service), QVariant(data));
    reply->onAllErrors(this, [domain, service](const QString& errorString, int, RestReplyErrorTypes::Enum, const QVariant&) {
        QVariantMap settings;
        settings.insert("caption", HassManager::tr("La commande Home Assistant %1.%2 a échoué").arg(domain, service));
        settings.insert("details", errorString);
        settings.insert("diagnose", false);
        SnackbarManager::Get()->showWarning(settings);
    });
}

void HassManager::setEntityEnabled(const QString& entityId, bool enabled)
{
    const QString domain = entityId.section('.', 0, 0);
    if(domain.isEmpty() || domain == entityId) {
        SnackbarManager::Get()->showWarning(tr("Entité Home Assistant invalide"));
        return;
    }

    callService(domain, enabled ? "turn_on" : "turn_off", {{"entity_id", entityId}});
}

void HassManager::activateScene(const QString& entityId)
{
    callService("scene", "turn_on", {{"entity_id", entityId}});
}

void HassManager::setInputTime(const QString& entityId, const QTime& time)
{
    if(!time.isValid()) {
        SnackbarManager::Get()->showWarning(tr("Heure Home Assistant invalide"));
        return;
    }

    callService("input_datetime", "set_datetime", {{"entity_id", entityId}, {"time", time.toString("HH:mm:ss")}});
}

void HassManager::setInputDate(const QString& entityId, const QDate& date)
{
    if(!date.isValid()) {
        SnackbarManager::Get()->showWarning(tr("Date Home Assistant invalide"));
        return;
    }

    callService("input_datetime", "set_datetime", {{"entity_id", entityId}, {"date", date.toString(Qt::ISODate)}});
}

void HassManager::setInputDateTime(const QString& entityId, const QDateTime& dateTime)
{
    if(!dateTime.isValid()) {
        SnackbarManager::Get()->showWarning(tr("Date et heure Home Assistant invalides"));
        return;
    }

    callService("input_datetime", "set_datetime", {{"entity_id", entityId}, {"datetime", dateTime.toString("yyyy-MM-dd hh:mm:ss")}});
}
