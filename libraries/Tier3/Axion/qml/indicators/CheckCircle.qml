import QtQuick
import Eco.Tier3.Axion

Item {
    id: root

    signal clicked()

    property bool checked: false

    anchors.horizontalCenter: parent.right
    anchors.verticalCenter: parent.top

    height: width
    width: 52

    Rectangle {
        id: circle
        color: Style.colorWhite
        height: 40
        width: height
        anchors.centerIn: parent
        radius: height/2

        states: State {
            name: "pressed"
            when: mouseArea.pressed
            PropertyChanges { circle.scale: 0.9 }
        }

        SvgColorImage {
            color: Style.colorBlack
            visible: root.checked
            anchors.centerIn: parent
            icon: MaterialIcons.check
            size: 24
        }
    }

    TapHandler {
        id: mouseArea
        target: circle
        onTapped: root.clicked()
    }
}
