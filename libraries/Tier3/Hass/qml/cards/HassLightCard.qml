import QtQuick
import Eco.Tier3.Axion
import Eco.Tier3.Hass

HassDelegate {
    id: root

    required property HassStateMapper mapper

    readonly property string entity_id: mapper.entity_id
    readonly property string state: mapper.state
    readonly property var attributes: mapper.attributes

    readonly property bool isUnavailable: state === "unavailable"
    readonly property bool isOn: state === "on"
    readonly property bool isGroup: HassHelper.isEntityGroup(attributes)
    readonly property int brightness: HassHelper.lightBrightness(attributes)
    readonly property int brightnessLevel: HassHelper.lightBrightnessLevel(attributes)
    readonly property color lightColor: HassHelper.lightColor(attributes)

    primaryText: attributes?.friendly_name ?? entity_id
    secondaryText: {
        if(isUnavailable)
            return qsTr("Indisponible")
        if(isGroup)
            return isOn ? qsTr("Groupe allumé") : qsTr("Groupe éteint")
        if(!isOn)
            return qsTr("Éteinte")
        if(brightness < 0)
            return qsTr("Allumée")
        return qsTr("%1 %").arg(brightness)
    }
    enabled: !isUnavailable
    active: isOn
    iconSource: {
        if(isGroup)
            return isOn ? MaterialIcons.lightbulbGroup : MaterialIcons.lightbulbGroupOff
        if(!isOn)
            return MaterialIcons.lightbulbOff

        switch(brightnessLevel) {
        case 0: return MaterialIcons.lightbulbOnOutline
        case 1: return MaterialIcons.lightbulbOn10
        case 2: return MaterialIcons.lightbulbOn20
        case 3: return MaterialIcons.lightbulbOn30
        case 4: return MaterialIcons.lightbulbOn40
        case 5: return MaterialIcons.lightbulbOn50
        case 6: return MaterialIcons.lightbulbOn60
        case 7: return MaterialIcons.lightbulbOn70
        case 8: return MaterialIcons.lightbulbOn80
        case 9: return MaterialIcons.lightbulbOn90
        default: return MaterialIcons.lightbulbOn
        }
    }
    iconColor: isOn && !isGroup && lightColor.a > 0 ? lightColor : Style.amber
}
