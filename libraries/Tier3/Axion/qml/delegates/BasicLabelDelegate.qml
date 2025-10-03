import QtQuick
import Eco.Tier3.Axion

BasicItemDelegate {
    id: root

    contentItem: Text {
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        font: root.font
        color: root.foregroundColor
        text: root.text
        wrapMode: root.wrapMode
        elide: root.elide
        textFormat: root.textFormat
    }
}
