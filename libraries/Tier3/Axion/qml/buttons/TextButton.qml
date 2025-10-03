import QtQuick
import Eco.Tier3.Axion

FlatButton {
    id: root

    contentItem: Text {
        text: root.text
        font: root.font
        color: root.foregroundColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
