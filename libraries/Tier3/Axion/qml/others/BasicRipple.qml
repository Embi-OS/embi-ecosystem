import QtQuick
import QtQuick.Controls.Material.impl as QQuickMaterial
import Eco.Tier3.Axion

QQuickMaterial.Ripple {
    id: root

    clip: true

    property bool focussed: false
    active: focussed

    property int radius: 0
    clipRadius: root.radius

    property color relativeBackgroundColor: Style.colorBackground
    property color rippleColor: ColorUtils.isDarkColor(root.relativeBackgroundColor) ? Style.rippleColorLight : Style.rippleColorDark

    color: rippleColor
}
