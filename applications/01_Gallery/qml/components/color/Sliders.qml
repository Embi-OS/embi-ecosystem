import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

ColumnLayout {
    id: root

    property real hue: 0.6
    property real saturation: 1.0
    property real value: 1.0
    property real alpha: 1.0

    readonly property color solidColor: ColorUtils.fromHsvF(hue, 1.0, 1.0, 1.0)
    readonly property color hueColor: ColorUtils.fromHsvF(hue, saturation, value, alphaEnabled ? alpha : 1.0)

    property real radius: 8
    property bool paletteEnabled: true
    property bool saturationEnabled: true
    property bool hueEnabled: true
    property bool alphaEnabled: alphaEnabled.checked
    property bool swatchEnabled: true
    property bool hexEnabled: true

    spacing: 0

    function readHsv(color: color) {
        root.hue = color.hsvHue
        root.saturation = color.hsvSaturation
        root.value = color.hsvValue
        root.alpha = color.a
    }

    ColorPaletteGrid {
        Layout.fillWidth: true
        visible: root.paletteEnabled
        squareRadius: root.radius
        currentColor: root.hueColor
        colorModel: StandardObjectModel {
            StandardObject { color: "#1105FB" }
            StandardObject { color: "#3B7D23" }
            StandardObject { color: "#FFFF00" }
            StandardObject { color: "#FF0000" }
            StandardObject { color: "#F905D6" }
            StandardObject { color: "#2DFFFF" }
            StandardObject { color: "#FC6E04" }
            StandardObject { color: "#8A18E8" }
        }
        onColorChosen: (color) => root.readHsv(color)
    }

    ColorSpectrum {
        Layout.fillWidth: true
        visible: root.saturationEnabled
        radius: root.radius
        hue: root.hue
        saturation: root.saturation
        value: root.value
        onMoved: (saturation, value) => {
            root.saturation = saturation
            root.value = value
        }
    }

    ColorHueSlider {
        Layout.fillWidth: true
        visible: root.hueEnabled
        radius: root.radius
        color: root.solidColor
        value: root.hue
        onMoved: root.hue = value
    }

    ColorAlphaSlider {
        Layout.fillWidth: true
        visible: root.alphaEnabled
        radius: root.radius
        color: root.solidColor
        value: root.alpha
        onMoved: root.alpha = value
    }

    ColorPreview {
        Layout.fillWidth: true
        visible: root.swatchEnabled
        swatchRadius: root.radius
        leftColor: root.solidColor
        rightColor: root.hueColor
        alphaEnabled: root.alphaEnabled
        onLeftClicked: root.readHsv(leftColor)
    }

    ColorHexField {
        Layout.fillWidth: true
        visible: root.hexEnabled
        color: root.hueColor
        alphaEnabled: root.alphaEnabled
        radius: root.radius
        onColorAccepted: root.readHsv(color)
    }

    BasicCheckBox {
        id: alphaEnabled
        text: "alphaEnabled"
        checked: true
    }

    BasicLabel {
        text: "hue: " + FormatUtils.realToString(root.hue, 2)
    }

    BasicLabel {
        text: "saturation: " + FormatUtils.realToString(root.saturation, 2)
    }

    BasicLabel {
        text: "value: " + FormatUtils.realToString(root.value, 2)
    }

    BasicLabel {
        text: "alpha: " + FormatUtils.realToString(root.alpha, 2)
    }

    BasicLabel {
        text: "color: " + ColorUtils.name(root.hueColor, true)
    }
}
