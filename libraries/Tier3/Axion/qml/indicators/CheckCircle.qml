import QtQuick
import Eco.Tier3.Axion

Rectangle {
    id: root

    implicitWidth: Style.checkCircleImplicitWidth
    implicitHeight: Style.checkCircleImplicitHeight

    radius: height/2
    border.width: 0
    border.color: Style.colorWhite
    color: Style.colorWhite

    opacity: enabled ? 1.0 : 0.5

    property color colorForeground: Style.colorBlack
    required property int checkState

    SvgColorImage {
        anchors.centerIn: parent
        size: Style.checkCircleIconSize
        icon: root.checkState === Qt.Checked ? MaterialIcons.check :
              root.checkState === Qt.PartiallyChecked ? MaterialIcons.minus : ""
        color: root.colorForeground
    }
}
