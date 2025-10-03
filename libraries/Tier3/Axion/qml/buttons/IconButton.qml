import QtQuick
import Eco.Tier3.Axion

FlatButton {
    id: root

    contentItem: SvgColorImage {
        icon: root.icon.source
        iconWidth: Style.roundToIconSize(root.icon.width)
        iconHeight: Style.roundToIconSize(root.icon.height)
        color: root.icon.color
        cache: root.icon.cache
    }
}
