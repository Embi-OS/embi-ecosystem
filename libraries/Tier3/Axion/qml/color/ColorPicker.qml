import QtQuick
import Eco.Tier3.Axion

ColumnLayout {
    id: root

    signal colorEdited(color color)

    property color selectedColor: Style.colorAccent

    property real hue: 0.6
    property real saturation: 1.0
    property real value: 1.0
    property real alpha: 1.0

    readonly property color solidColor: ColorUtils.fromHsvF(hue, 1.0, 1.0, 1.0)
    readonly property color hueColor: ColorUtils.fromHsvF(hue, saturation, value, alphaEnabled ? alpha : 1.0)
    readonly property color rgbColor: ColorUtils.toRgb(hueColor)

    property bool paletteEnabled: true
    property bool saturationEnabled: true
    property bool hueEnabled: true
    property bool alphaEnabled: true
    property bool swatchEnabled: true
    property bool hexEnabled: true

    property AbstractItemModel colorModel: null

    property real radius: 8
    spacing: 0

    onSelectedColorChanged: {
        readHsv(selectedColor)
    }

    onRgbColorChanged: {
        root.colorEdited(root.rgbColor)
    }

    function readHsv(color: color) {
        root.hue = color.hsvHue
        root.saturation = color.hsvSaturation
        root.value = color.hsvValue
        root.alpha = color.a
    }

    ColorPaletteGrid {
        Layout.fillWidth: true
        visible: root.paletteEnabled && count>=1
        squareRadius: root.radius
        currentColor: root.hueColor
        colorModel: root.colorModel
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
        leftColor: root.selectedColor
        rightColor: root.hueColor
        alphaEnabled: root.alphaEnabled
        onLeftClicked: root.readHsv(leftColor)
    }

    ColorHexField {
        Layout.fillWidth: true
        visible: root.hueEnabled
        color: root.hueColor
        alphaEnabled: root.alphaEnabled
        radius: root.radius
        onColorAccepted: (color) => root.readHsv(color)
    }
}
