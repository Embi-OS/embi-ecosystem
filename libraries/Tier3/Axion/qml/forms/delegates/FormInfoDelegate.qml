pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

FormObject {
    id: root

    formType: FormTypes.None
    defaultValue: ""

    font: Style.textTheme.subtitle1
    property int type: InfoObjectTypes.Normal

    delegate: ListInfosRow {
        fitLabel: root.fitLabel
        labelWidthRatio: root.labelWidthRatio

        enabled: root.enabled
        type: root.type
        label: root.label
        info: root.valueToText(root.currentValue)
        font: root.font
    }
}
