import QtQuick
import Eco.Tier3.Axion
import Eco.Tier3.Hass

HassDelegate {
    id: root

    property string entity_id: ""
    property string state: ""
    property var attributes: ({})

    readonly property bool isUnavailable: state === "unavailable"

    primaryText: attributes?.friendly_name ?? entity_id
    secondaryText: entity_id
    enabled: !isUnavailable
    iconSource: {
        switch(state) {
        case "on": return MaterialIcons.powerOn
        case "off": return MaterialIcons.powerOff
        case "unavailable": return MaterialIcons.alertCircleOutline
        case "unknown": return MaterialIcons.helpCircleOutline
        default: return MaterialIcons.template_
        }
    }
}
