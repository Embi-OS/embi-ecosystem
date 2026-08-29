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
    enabled: !isUnavailable
    iconSource: isUnavailable ? MaterialIcons.alertCircleOutline : MaterialIcons.paletteOutline
}
