pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

FormObject {
    id: root

    formType: FormTypes.TextArea
    defaultValue: ""

    delegate: FormTextArea {
        fitLabel: root.fitLabel
        labelWidthRatio: root.labelWidthRatio
        editable: root.editable

        enabled: root.enabled
        warning: root.warning

        label: root.label
        infos: root.infos
        placeholder: root.placeholder
        text: root.currentValue

        onAccepted: (text) => root.changeValue(text)
    }
}
