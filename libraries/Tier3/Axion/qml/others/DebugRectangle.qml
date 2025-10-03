import QtQuick
import Eco.Tier3.Axion

Rectangle {
    id: root

    z: Infinity
    anchors.fill: parent
    color: "transparent"
    border.width: 1
    border.color: ColorUtils.blend(RandomUtils.color(), Style.white, 0.5)
}
