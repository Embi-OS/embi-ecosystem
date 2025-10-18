pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

FormObject {
    id: root

    formType: FormTypes.None
    defaultValue: ""

    font: Style.textTheme.subtitle1
    highlighted: false
    property int wrapMode: Text.NoWrap
    property int elide: Text.ElideRight
    property int textFormat: Text.PlainText

    delegate: BasicLabel {
        enabled: root.enabled
        text: (root.label!=="" ? (root.label+": ") : "") + root.valueToText(root.currentValue)
        font: root.font
        highlighted: root.highlighted
        wrapMode: root.wrapMode
        elide: root.elide
        textFormat: root.textFormat
    }
}
