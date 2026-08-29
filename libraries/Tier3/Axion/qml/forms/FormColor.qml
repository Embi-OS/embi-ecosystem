import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

BasicFormBackground {
    id: root

    signal accepted(color color)

    required property AbstractItemModel colorsModel

    property alias color: rectangle.color
    readonly property string colorText: ColorUtils.name(root.color, false)

    property bool paletteEnabled: true
    property bool saturationEnabled: true
    property bool hueEnabled: true
    property bool alphaEnabled: false
    property bool swatchEnabled: false
    property bool hexEnabled: true

    function showColorDialog() {
        DialogManager.showColor({
            selectedColor: color,
            colorModel: root.colorsModel,
            paletteEnabled: root.paletteEnabled,
            saturationEnabled: root.saturationEnabled,
            hueEnabled: root.hueEnabled,
            alphaEnabled: root.alphaEnabled,
            swatchEnabled: root.swatchEnabled,
            hexEnabled: root.hexEnabled,
            onColorSelected: function(color) {
                root.color = color;
                root.accepted(root.color);
            }
        });
    }

    Item {
        parent: root.controlRectangle
        anchors.fill: parent

        Rectangle {
            id: rectangle
            anchors.fill: parent
            anchors.margins: Style.formBorderWidth

            TapHandler {
                id: tapHandler
                enabled: root.editable
                onTapped: root.showColorDialog()
            }
        }

        Text {
            id: placeholder
            anchors.fill: parent
            anchors.margins: Style.formInnerMargin
            color: ColorUtils.transparent(ColorUtils.isDarkColor(root.color) ? Style.colorWhite : Style.colorBlack, 0.6)
            text: root.colorText
            font: root.font
            elide: Text.ElideRight
        }
    }
}
