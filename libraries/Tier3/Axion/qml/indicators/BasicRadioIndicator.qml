import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

Rectangle {
    id: root

    implicitWidth: Style.radioIndicatorImplicitWidth
    implicitHeight: Style.radioIndicatorImplicitHeight

    radius: width / 2
    border.width: Style.radioIndicatorBorderWidth
    border.color: control.checked || control.down ? Style.colorAccent : colorForeground
    color: "transparent"

    opacity: enabled ? 1.0 : 0.5

    property color colorForeground: ColorUtils.isDarkColor(relativeBackgroundColor) ? Style.colorWhite : Style.colorBlack
    property color relativeBackgroundColor: Style.colorBackground
    required property T.AbstractButton control

    Rectangle {
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        visible: root.control.checked
        width: root.width / 2
        height: root.height / 2
        radius: width / 2
        color: parent.border.color
    }
}
