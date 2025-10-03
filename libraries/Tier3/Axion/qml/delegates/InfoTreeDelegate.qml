pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

StandardObject {
    id: root

    font: Style.textTheme.subtitle1
    text: ""
    property string info: ""
    property int type: InfoObjectTypes.Normal

    delegate: ListInfosRow {
        type: root.type
        label: root.text
        info: root.info
        font: root.font
    }
}
