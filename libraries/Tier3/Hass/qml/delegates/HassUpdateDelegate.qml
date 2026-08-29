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
    readonly property bool isAvailable: state === "on"
    readonly property string latestVersion: attributes?.latest_version ?? ""
    readonly property string installedVersion: attributes?.installed_version ?? ""

    primaryText: attributes?.title ?? attributes?.friendly_name ?? entity_id
    secondaryText: {
        if(isUnavailable)
            return qsTr("Indisponible")
        if(isUnknown)
            return qsTr("État inconnu")
        if(isAvailable && latestVersion.length > 0)
            return qsTr("Version %1 disponible").arg(latestVersion)
        if(installedVersion.length > 0)
            return qsTr("À jour : %1").arg(installedVersion)
        return isAvailable ? qsTr("Mise à jour disponible") : qsTr("À jour")
    }
    active: isAvailable
    iconColor: Style.amber
    iconSource: {
        if(isUnavailable)
            return MaterialIcons.alertCircleOutline
        if(isUnknown)
            return MaterialIcons.helpCircleOutline
        return isAvailable ? MaterialIcons.packageUp : MaterialIcons.packageCheck
    }
}
