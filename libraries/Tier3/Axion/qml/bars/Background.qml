import QtQuick
import Eco.Tier3.Axion

Rectangle {
    id: root
    layer.enabled: true

    color: ColorUtils.transparent(Style.colorBackground,1.0)

    Image {
        anchors.fill: parent
        asynchronous: true
        fillMode: Style.backgroundAspect
        source: Style.backgroundImage
    }
}
