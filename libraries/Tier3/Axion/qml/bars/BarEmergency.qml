import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.Popup {
    id: root

    property int barHeight: root.height/4

    property int labelSize: barHeight/2
    property color labelColor: Style.colorBlack
    property color barColor: Style.colorWarning

    anchors.centerIn: parent
    height: parent.height
    width: parent.width
    closePolicy: T.Popup.NoAutoClose

    modal: true // Back darker
    focus: true

    T.Overlay.modal: BasicOverlay {}
    T.Overlay.modeless: BasicOverlay {}

    enter: Transition {
      NumberAnimation { property: "scale";from: 0.9;to: 1.0;easing.type: Easing.OutQuint;duration: 220 }
      NumberAnimation { property: "opacity";from: 0.0;to: 1.0;easing.type: Easing.OutCubic;duration: 150 }
    }

    exit: Transition {
      NumberAnimation { property: "scale";from: 1.0;to: 0.9;easing.type: Easing.OutQuint;duration: 220 }
      NumberAnimation { property: "opacity";from: 1.0;to: 0.0;easing.type: Easing.OutCubic;duration: 150 }
    }

    background: Rectangle {
        color: "transparent"
    }

    Rectangle {
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        height: root.barHeight

        color: root.barColor

        BasicLabel {
            id: labelItem
            anchors.centerIn: parent
            fontSizeMode: Text.VerticalFit
            font.bold: true
            font.pixelSize: root.labelSize
            font.family: Style.primaryFont
            text: qsTr("Arrêt d'urgence !")
            color: ColorUtils.isDarkColor(root.barColor) ? Style.colorWhite : Style.colorBlack
        }
    }
}
