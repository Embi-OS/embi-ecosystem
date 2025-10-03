import QtQuick
import QtQuick.Controls.Material.impl as QQuickMaterial
import Eco.Tier3.Axion

// TODO: Qt 6.9 RectangularShadow
QQuickMaterial.RectangularGlow {
    id: root

    property alias center: center

    Rectangle {
        id: center
        anchors.fill: parent
        radius: height/2
        color: root.color
    }
}
