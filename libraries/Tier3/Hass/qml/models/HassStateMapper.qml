import QtQuick
import Eco.Tier1.Utils
import Eco.Tier3.Hass

ModelMapper {
    id: root

    baseName: "HassStateMapper"

    roleName: "entity_id"
    selectWhen: hassModel && hassModel.selectWhen && hassModel.selected
    selectPolicy: QVariantMapperPolicies.Delayed
    submitPolicy: QVariantMapperPolicies.Disabled

    readonly property HassEntityModel hassModel: model as HassEntityModel

    property string entity_id: ""
    property string state: ""
    property var attributes: ({})
    property date last_changed: new Date()
}
