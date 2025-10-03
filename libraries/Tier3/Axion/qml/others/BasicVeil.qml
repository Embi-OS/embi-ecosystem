import QtQuick
import Eco.Tier3.Axion

Rectangle {
    id: root

    visible: (focussed || pressed) && enabled

    property bool focussed: false
    property bool pressed: false

    readonly property Style style: Style

    property color relativeBackgroundColor: Style.colorBackground
    property color rippleColor: ColorUtils.isDarkColor(root.relativeBackgroundColor) ? Style.rippleColorLight : Style.rippleColorDark

    color: pressed ? ColorUtils.transparent(rippleColor, 0.4) :
           focussed ? ColorUtils.transparent(rippleColor, 0.2) : Style.colorTransparent
    radius: 0
}
