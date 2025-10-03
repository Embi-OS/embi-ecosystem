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

    warning: standardObjectGroup.warning
    isDirty: standardObjectGroup.isDirty
    readonly property StandardObjectGroup standardObjectGroup: StandardObjectGroup {
        delayed: true
        enabledWarning: true
        enabledDirty: true
    }
    onObjectInserted: (object, signalIndex) => standardObjectGroup.addObject(object)
    onObjectRemoved: (object, signalIndex) => standardObjectGroup.removeObject(object)

    delegate: BasicTreeDelegate {
        color: root.warning ? Style.colorWarning :
               root.highlighted ? Style.colorAccent : Style.colorWhite

        text: root.text
        font: root.font

        highlighted: root.highlighted
        wrapMode: root.wrapMode
        elide: root.elide
        textFormat: root.textFormat

        onClicked: root.toggle()
    }
}
