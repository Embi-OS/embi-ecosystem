import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

BasicFormBackground {
    id: root

    signal accepted(color color)

    required property AbstractItemModel colorsModel

    property bool displayName: false
    property alias color: rectangle.color
    readonly property string colorText: ColorUtils.name(root.color, false)
    readonly property string colorName: currentIndex>0 ? root.colorsModel.ModelHelper.getProperty(currentIndex, "text") : colorText
    readonly property int currentIndex: root.colorsModel.ModelHelper.contentIsEmpty ? -1 : root.colorsModel.ModelHelper.indexOf("color",root.color)

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
                onTapped: colorPicker.open()
            }
        }

        Text {
            id: placeholder
            anchors.fill: parent
            anchors.margins: Style.formInnerMargin
            color: ColorUtils.transparent(ColorUtils.isDarkColor(root.color) ? Style.colorWhite : Style.colorBlack, 0.6)
            text: root.displayName ? root.colorName : root.colorText
            font: root.font
            elide: Text.ElideRight
        }
    }

    PopupColorPicker {
        id: colorPicker
        ExtraPosition.position: ItemPositions.BottomEnd

        colorModel: root.colorsModel
        currentColor: root.color
        onColorChosen: (color) => root.accepted(color)
    }
}
