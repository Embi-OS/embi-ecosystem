import QtQuick
import Eco.Tier3.Axion

BasicFormBackground {
    id: root

    signal activated(bool checked)

    property alias leftLabel: leftLabel.text
    property alias rightLabel: rightLabel.text
    property bool checked: false

    Item {
        parent: root.controlRectangle
        anchors.fill: parent
        anchors.margins: Style.formInnerMargin

        BasicLabel {
            id: leftLabel
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.right: rect.left
            anchors.rightMargin: Style.formInnerMargin/2 + round.width/2
            font: root.font
            horizontalAlignment: Qt.AlignLeft
            elide: Text.ElideRight
        }

        Rectangle {
            id: rect
            width: 49
            height: 2
            anchors.centerIn: parent
            radius: height/2
            color: Style.colorWhite

            Rectangle {
                id: round
                anchors.verticalCenter: parent.verticalCenter
                x: root.checked ? parent.width-(width/2) : -(width/2)
                width: 18
                height: width
                radius: width/2
                color: root.checked ? Style.colorAccent : Style.colorWhite

                Behavior on x {
                    SmoothedAnimation {duration: 250}
                }
            }
        }

        BasicLabel {
            id: rightLabel
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: rect.right
            anchors.right: parent.right
            anchors.leftMargin: Style.formInnerMargin/2 + round.width/2
            font: root.font
            horizontalAlignment: Qt.AlignRight
            elide: Text.ElideRight
            maximumLineCount: 1
        }

        TapHandler {
            id: tapHandler
            gesturePolicy: TapHandler.ReleaseWithinBounds
            cursorShape: Qt.PointingHandCursor
            enabled: root.editable
            onTapped: {
                root.activated(!root.checked)
            }
        }
    }
}
