pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion
import Eco.Tier3.Hass

HassDelegate {
    id: root

    required property HassStateMapper mapper

    signal brightnessRequested(int brightness)
    signal colorTemperatureRequested(int kelvin)
    signal colorRequested(int hue, int saturation)

    property bool useLightColor: true
    property bool showBrightnessControl: true
    property bool showColorControl: true
    property bool showColorTempControl: true
    property bool collapsibleControls: false
    property string controlMode: "brightness"

    readonly property string entity_id: mapper.entity_id
    readonly property string state: mapper.state
    readonly property var attributes: mapper.attributes
    readonly property bool isUnavailable: !entity_id || (state !== "on" && state !== "off")
    readonly property bool isOn: state === "on"
    readonly property bool isGroup: HassHelper.isEntityGroup(attributes)
    readonly property int brightness: HassHelper.lightBrightness(attributes)
    readonly property int brightnessLevel: HassHelper.lightBrightnessLevel(attributes)
    readonly property color lightColor: HassHelper.lightColor(attributes)
    readonly property int minKelvin: Math.max(1000, attributes.min_color_temp_kelvin || 2000)
    readonly property int maxKelvin: Math.max(minKelvin, attributes.max_color_temp_kelvin || 6500)
    readonly property int temperature: Math.max(minKelvin, Math.min(maxKelvin, attributes.color_temp_kelvin || minKelvin))
    readonly property real hue: attributes.hs_color?.length === 2 ? Math.max(0, Math.min(360, attributes.hs_color[0])) : Math.max(0, lightColor.hsvHue * 360)
    readonly property list<string> controlModes: {
        const modes = []
        if(showBrightnessControl && HassHelper.lightSupportsBrightness(attributes))
            modes.push("brightness")
        if(showColorTempControl && (attributes.supported_color_modes ?? []).indexOf("color_temp") >= 0
                && maxKelvin > minKelvin)
            modes.push("temperature")
        if(showColorControl && HassHelper.lightSupportsColor(attributes))
            modes.push("color")
        return modes
    }
    readonly property string activeControlMode: controlModes.indexOf(controlMode) >= 0 ? controlMode : (controlModes[0] ?? "")
    readonly property real reportedPosition: {
        switch(activeControlMode) {
        case "temperature": return (temperature - minKelvin) / Math.max(1, maxKelvin - minKelvin)
        case "color": return hue / 360
        default: return isOn ? Math.max(0, brightness) / 100 : 0
        }
    }
    readonly property bool controlsVisible: controlModes.length > 0 && (!collapsibleControls || isOn)
    readonly property bool stacked: availableWidth < 420

    enabled: !isUnavailable
    active: isOn
    primaryText: attributes.friendly_name ?? entity_id
    secondaryText: {
        if(isUnavailable)
            return state === "unavailable" ? qsTr("Indisponible") : qsTr("Inconnue")
        if(!isOn)
            return isGroup ? qsTr("Groupe éteint") : qsTr("Éteinte")
        if(brightness >= 0)
            return qsTr("%1 %").arg(brightness)
        return isGroup ? qsTr("Groupe allumé") : qsTr("Allumée")
    }
    iconSource: isGroup ? (isOn ? MaterialIcons.lightbulbGroup : MaterialIcons.lightbulbGroupOff) :
                          (isOn ? MaterialIcons.lightbulbOn : MaterialIcons.lightbulbOff)
    iconColor: useLightColor && isOn && lightColor.a > 0 ? lightColor : Style.amber

    function modeName(mode: string): string {
        switch(mode) {
        case "temperature": return qsTr("Température")
        case "color": return qsTr("Couleur")
        default: return qsTr("Intensité")
        }
    }

    onEntity_idChanged: {
        if(slider)
            slider.resetControl()
    }

    contentItem: GridLayout {
        columns: root.stacked ? 1 : 2
        columnSpacing: 12
        rowSpacing: 4

        RowLayout {
            Layout.fillWidth: true
            Layout.minimumWidth: 120
            spacing: 12

            Rectangle {
                Layout.preferredWidth: 48
                Layout.preferredHeight: 48
                radius: height / 2
                color: root.active ? ColorUtils.transparent(root.iconColor, 0.2) : Style.colorPrimaryDark

                SvgColorImage {
                    anchors.centerIn: parent
                    size: 28
                    icon: root.iconSource
                    color: root.active ? root.iconColor : root.foregroundColor
                }
            }

            LabelWithCaption {
                Layout.fillWidth: true
                spacing: 2
                textFont: Style.textTheme.subtitle2
                textColor: root.foregroundColor
                text: root.primaryText
                captionFont: Style.textTheme.caption1
                captionColor: ColorUtils.transparent(root.foregroundColor, 0.7)
                caption: root.secondaryText
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredWidth: 250
            spacing: 4
            visible: root.controlsVisible

            HassLightSlider {
                id: slider
                radius: 12
                Layout.fillWidth: true
                Layout.minimumWidth: 100
                Layout.preferredHeight: 48
                controlMode: root.activeControlMode
                reportedPosition: root.reportedPosition
                minKelvin: root.minKelvin
                maxKelvin: root.maxKelvin
                color: root.iconColor
                foregroundColor: root.foregroundColor
                onBrightnessRequested: (brightness) => root.brightnessRequested(brightness)
                onColorTemperatureRequested: (kelvin) => root.colorTemperatureRequested(kelvin)
                onColorRequested: (hue, saturation) => root.colorRequested(hue, saturation)
            }

            Repeater {
                model: root.controlModes

                IconButton {
                    required property string modelData
                    inset: 0
                    radius: 12
                    Layout.preferredWidth: 44
                    Layout.preferredHeight: 48
                    visible: modelData !== root.activeControlMode
                    checkable: false
                    icon.source: modelData === "temperature" ? MaterialIcons.thermometer
                        : modelData === "color" ? MaterialIcons.palette : MaterialIcons.brightness6
                    backgroundColor: Style.colorPrimaryDark
                    onClicked: root.controlMode = modelData
                }
            }
        }
    }
}
