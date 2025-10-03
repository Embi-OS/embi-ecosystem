import QtQuick
import Eco.Tier3.Axion

Rectangle {
    id: root

    implicitWidth: Style.checkIndicatorImplicitWidth
    implicitHeight: Style.checkIndicatorImplicitHeight

    radius: Style.checkIndicatorRadius
    border.width: Style.checkIndicatorBorderWidth
    border.color: checkState !== Qt.Unchecked ? Style.colorAccent : colorForeground
    color: checkState !== Qt.Unchecked ? Style.colorAccent : "transparent"

    opacity: enabled ? 1.0 : 0.5

    property color colorForeground: ColorUtils.isDarkColor(relativeBackgroundColor) ? Style.colorWhite : Style.colorBlack
    property color relativeBackgroundColor: Style.colorBackground
    required property int checkState

    SvgColorImage {
        anchors.centerIn: parent
        size: Style.checkIndicatorIconSize
        icon: root.checkState === Qt.Checked ? MaterialIcons.check :
              root.checkState === Qt.PartiallyChecked ? MaterialIcons.minus : ""
        color: root.colorForeground
    }
}
