import QtQuick
import Eco.Tier3.Axion
import Eco.Tier3.Hass

HassDelegate {
    id: root

    required property HassStateMapper temperatureMapper
    required property HassStateMapper humidityMapper

    readonly property string temperature_entity_id: root.temperatureMapper.entity_id
    readonly property string temperature_state: root.temperatureMapper.state
    readonly property var temperature_attributes: root.temperatureMapper.attributes

    readonly property string humidity_entity_id: root.humidityMapper.entity_id
    readonly property string humidity_state: root.humidityMapper.state
    readonly property var humidity_attributes: root.humidityMapper.attributes

    readonly property bool isUnavailable: temperature_state==="unavailable" || humidity_state==="unavailable"
    readonly property bool isUnknown: temperature_state==="unknown" || humidity_state==="unknown"
    readonly property string temperatureUnit: temperature_attributes?.unit_of_measurement ?? ""
    readonly property string humidityUnit: humidity_attributes?.unit_of_measurement ?? ""
    readonly property real temperatureValue: temperature_state
    readonly property color temperatureIconColor: {
        if(temperatureValue !== temperatureValue)
            return Style.cyan
        if(temperatureValue < 10)
            return "purple"
        if(temperatureValue < 19)
            return "#dbeff0"
        if(temperatureValue < 22)
            return "#2196f3"
        if(temperatureValue < 24)
            return "#d6bb57"
        if(temperatureValue < 26)
            return "#c6623e"
        if(temperatureValue < 28)
            return "#ac4231"
        return "red"
    }

    primaryText: temperature_attributes?.friendly_name ?? temperature_entity_id
    secondaryText: qsTr("%1 %2 - %3 %4").arg(temperature_state).arg(temperatureUnit).arg(humidity_state).arg(humidityUnit)
    active: !isUnavailable && !isUnknown
    iconColor: temperatureIconColor
    iconSource: {
        if(isUnavailable)
            return MaterialIcons.alertCircleOutline
        if(isUnknown)
            return MaterialIcons.helpCircleOutline
        return MaterialIcons.thermometer
    }
}
