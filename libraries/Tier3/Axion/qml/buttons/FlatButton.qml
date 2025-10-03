import QtQuick
import Eco.Tier3.Axion

RawButton {
    id: root

    background: Rectangle {
        implicitWidth: root.backgroundImplicitWidth
        implicitHeight: root.backgroundImplicitHeight

        radius: root.radius
        color: root.backgroundColor

        BasicVeil {
            z: 100
            anchors.fill: parent
            radius: root.radius

            relativeBackgroundColor: root.relativeBackgroundColor
            focussed: root.down || root.visualFocus || root.hovered
            pressed: root.pressed
        }
    }
}
