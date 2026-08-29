import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

Rectangle {
    id: root

    implicitWidth: 18
    implicitHeight: 38
    radius: width / 2

    color: "transparent"
    border.color: Style.colorWhite
    border.width: 2

    property alias handleColor: circle.color

    Rectangle {
        id: circle
        anchors.fill: parent
        anchors.margins: root.border.width
        radius: width/2
        color: "transparent"
        border.width: root.border.width/2
        border.color: Style.colorBlack
    }
}
