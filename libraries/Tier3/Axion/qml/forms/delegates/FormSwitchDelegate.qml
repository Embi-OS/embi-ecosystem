pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

FormObject {
    id: root

    formType: FormTypes.Switch
    defaultValue: false

    property string leftLabel: ""
    property string rightLabel: ""

    readonly property bool checked: currentValue

    delegate: FormSwitch {
        fitLabel: root.fitLabel
        labelWidthRatio: root.labelWidthRatio
        editable: root.editable

        enabled: root.enabled
        warning: root.warning

        label: root.label
        infos: root.infos
        placeholder: root.placeholder
        leftLabel: root.leftLabel
        rightLabel: root.rightLabel
        checked: root.currentValue

        onActivated: (checked) => root.changeValue(checked)
    }
}
