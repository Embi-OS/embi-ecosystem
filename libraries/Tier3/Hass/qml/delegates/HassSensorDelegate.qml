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
    readonly property string unit: attributes?.unit_of_measurement ?? ""
    readonly property string deviceClass: attributes?.device_class ?? ""

    primaryText: attributes?.friendly_name ?? entity_id
    secondaryText: {
        if(isUnavailable)
            return qsTr("Indisponible")
        if(isUnknown)
            return qsTr("État inconnu")
        return unit.length > 0 ? qsTr("%1 %2").arg(state).arg(unit) : state
    }
    active: !isUnavailable && !isUnknown
    iconColor: {
        switch(deviceClass) {
        case "temperature":
        case "apparent_temperature": return Style.red
        case "humidity":
        case "moisture": return Style.blue
        case "battery": return Style.green
        case "power":
        case "energy":
        case "voltage":
        case "current": return Style.amber
        case "timestamp": return Style.purple
        default: return Style.cyan
        }
    }
    iconSource: {
        if(isUnavailable)
            return MaterialIcons.alertCircleOutline
        if(isUnknown)
            return MaterialIcons.helpCircleOutline

        switch(deviceClass) {
        case "temperature":
        case "apparent_temperature": return MaterialIcons.thermometer
        case "humidity":
        case "moisture": return MaterialIcons.waterPercent
        case "battery": return MaterialIcons.battery
        case "power":
        case "energy":
        case "voltage":
        case "current": return MaterialIcons.lightningBolt
        case "timestamp": return MaterialIcons.clockOutline
        default: return MaterialIcons.chartLine
        }
    }
}
