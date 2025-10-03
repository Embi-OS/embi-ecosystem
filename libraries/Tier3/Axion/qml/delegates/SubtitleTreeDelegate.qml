pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

StandardObject {
    id: root

    text: ""
    font: Style.textTheme.title2
    highlighted: false
    property int wrapMode: Text.NoWrap
    property int elide: Text.ElideRight
    property int textFormat: Text.PlainText

    delegate: BasicTreeDelegate {
        text: root.text
        font: root.font

        highlighted: root.highlighted
        wrapMode: root.wrapMode
        elide: root.elide
        textFormat: root.textFormat

        onClicked: root.toggle()
    }
}
