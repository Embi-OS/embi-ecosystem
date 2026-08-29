#include "hassentitymodel.h"
#include "hass_helpertypes.h"

HassEntityModel::HassEntityModel(QObject* parent):
    RestModel("api/states", parent)
{
    addAdditionalRole("domain", {"entity_id"}, [](const QVariant& entity){
        const QVariantMap& original = *reinterpret_cast<const QVariantMap*>(entity.constData());
        const QString entityId = original.value("entity_id").toString();
        return entityId.section('.', 0, 0);
    });

    m_primaryField = "entity_id";
    m_syncable = true;

    m_selectPolicy = QVariantListModelPolicies::Delayed;
    m_submitPolicy = QVariantListModelPolicies::Disabled;

    setPlaceholder(HassHelper::defaultStateMap());
}

QSWorker* HassEntityModel::createSubmitWorker()
{
    return nullptr;
}
