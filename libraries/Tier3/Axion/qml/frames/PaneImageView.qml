import QtQuick
import Eco.Tier3.Axion

BasicPane {
    id: root

    property string imageSource: ""

    contentItem: SvgImage {
        icon: root.imageSource
        iconWidth: width
        iconHeight: height
        aspectRatio: Qt.KeepAspectRatio
    }
}
