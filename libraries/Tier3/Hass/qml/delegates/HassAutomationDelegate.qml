import QtQuick
import Eco.Tier3.Axion
import Eco.Tier3.Hass

HassDelegate {
    id: root

    property string entity_id: ""
    property string state: ""
    property var attributes: ({})

    readonly property bool isUnavailable: state === "unavailable"
    readonly property bool isUnknown: state === "unknown"
    readonly property bool isOn: state === "on"

    primaryText: attributes?.friendly_name ?? entity_id
    secondaryText: {
        if(isUnavailable)
            return qsTr("Indisponible")
        if(isUnknown)
            return qsTr("État inconnu")
        return isOn ? qsTr("Activée") : qsTr("Désactivée")
    }
    enabled: !isUnavailable
    active: isOn
    iconColor: Style.teal
    iconSource: {
        if(isUnavailable)
            return MaterialIcons.alertCircleOutline
        if(isUnknown)
            return MaterialIcons.helpCircleOutline
        return isOn ? MaterialIcons.cogPlay : MaterialIcons.cogPlayOutline
    }
}
