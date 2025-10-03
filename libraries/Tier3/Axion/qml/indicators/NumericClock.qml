import QtQuick
import Eco.Tier3.Axion

RowLayout {
    id: root

    implicitHeight: 32

    property color relativeBackgroundColor: Style.colorBackground
    property color baseColor: ColorUtils.isDarkColor(relativeBackgroundColor) ? Style.colorWhite : Style.colorBlack
    readonly property color color: ColorUtils.transparent(baseColor, 1.0)
    property string font: "DSEG7 Classic"
    property int fontWeight: Font.Normal
    property int size: 32

    property int hour: 12
    property int minute: 24
    property int second: 35

    property alias secondeVisible: labelSeconde.visible

    spacing: 0

    Text {
        id: labelHour
        Layout.fillHeight: true
        Layout.alignment: Qt.AlignBottom
        text: FormatUtils.intToString(root.hour,2)
        font.family: root.font
        font.weight: root.fontWeight
        font.pixelSize: root.size
        color: root.color
    }
    Text {
        id: labelSeparate
        Layout.fillHeight: true
        Layout.alignment: Qt.AlignBottom
        text: ":"
        font.family: root.font
        font.weight: root.fontWeight
        font.pixelSize: root.size
        color: root.color
    }
    Text {
        id: labelMinute
        Layout.fillHeight: true
        Layout.alignment: Qt.AlignBottom
        text: FormatUtils.intToString(root.minute,2)
        font.family: root.font
        font.weight: root.fontWeight
        font.pixelSize: root.size
        color: root.color
    }
    Text {
        id: labelSeconde
        Layout.fillHeight: true
        Layout.alignment: Qt.AlignBottom
        text: FormatUtils.intToString(root.second,2)
        font.family: root.font
        font.weight: root.fontWeight
        font.pixelSize: root.size/5
        color: root.color
        verticalAlignment: Text.AlignBottom
    }
}
