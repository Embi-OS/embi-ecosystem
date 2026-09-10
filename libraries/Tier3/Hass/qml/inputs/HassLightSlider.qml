import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion
import Eco.Tier3.Hass

T.Slider {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitHandleWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitHandleHeight + topPadding + bottomPadding)

    from: 0.0
    to: 1.0
    padding: 0

    stepSize: root.controlMode === "color" ? 1 / 360 : root.controlMode === "temperature" ? 1 / Math.max(1, root.maxKelvin - root.minKelvin) : 0.01
    live: true

    property string controlMode: "brightness"
    property real reportedPosition: 0
    property int minKelvin: 2000
    property int maxKelvin: 6500
    property color color: Style.amber
    property color foregroundColor: Style.colorWhite

    property int radius: 12

    signal brightnessRequested(int brightness)
    signal colorTemperatureRequested(int kelvin)
    signal colorRequested(int hue, int saturation)

    property bool dirty: false
    readonly property int displayValue: {
        switch(root.controlMode) {
        case "temperature": return Math.round(root.minKelvin + value * (root.maxKelvin - root.minKelvin))
        case "color": return Math.round(value * 360)
        default: return Math.round(value * 100)
        }
    }
    readonly property string valueText: {
        switch(root.controlMode) {
        case "temperature": return qsTr("%1 K").arg(displayValue)
        case "color": return qsTr("%1°").arg(displayValue)
        default: return qsTr("%1 %").arg(displayValue)
        }
    }

    Binding on value {
        when: !root.pressed && !root.dirty && !settleTimer.running
        value: root.reportedPosition
        restoreMode: Binding.RestoreNone
    }

    function submit(): void {
        if(!dirty)
            return
        if(!root.enabled || !root.visible) {
            dirty = false
            return
        }

        const selectedValue = displayValue
        settleTimer.restart()
        dirty = false
        switch(root.controlMode) {
        case "brightness":
            root.brightnessRequested(selectedValue)
            break
        case "temperature":
            root.colorTemperatureRequested(selectedValue)
            break
        // A hue slider selects saturated colors; brightness stays independent.
        case "color":
            root.colorRequested(selectedValue, 100)
            break
        }
    }

    function resetControl(): void {
        dirty = false
        if(settleTimer)
            settleTimer.stop()
    }

    onControlModeChanged: resetControl()
    onEnabledChanged: resetControl()
    onVisibleChanged: resetControl()

    // Keep the local value briefly while waiting for the entity's state update.
    Timer {
        id: settleTimer
        interval: 1500
    }

    onMoved: {
        dirty = true
        if(!pressed)
            submit()
    }
    onPressedChanged: {
        if(!pressed)
            submit()
    }

    handle: ColorPickerHandle {
        x: root.leftPadding + root.visualPosition * (root.availableWidth - width)
        y: root.topPadding + root.availableHeight / 2 - height / 2
        visible: root.controlMode !== "brightness" || root.visualFocus || root.pressed
    }

    background: Rectangle {
        x: root.leftPadding + root.implicitHandleWidth / 2
        y: root.topPadding + (root.availableHeight - height) / 2
        implicitWidth: 200
        implicitHeight: 32
        width: Math.max(0, root.availableWidth - root.implicitHandleWidth)
        height: root.availableHeight
        radius: root.radius
        color: ColorUtils.transparent(root.color, 0.2)
        gradient: root.controlMode === "color" ? hueGradient : root.controlMode === "temperature" ? temperatureGradient : null

        ColorHueGradient {
            id: hueGradient
            orientation: Gradient.Horizontal
        }
        ColorTemperatureGradient {
            id: temperatureGradient
            orientation: Gradient.Horizontal
            minKelvin: root.minKelvin
            maxKelvin: root.maxKelvin
        }

        Rectangle {
            width: parent.width * root.position
            height: parent.height
            radius: root.radius
            color: root.color
            visible: root.controlMode === "brightness"
        }
    }
}
