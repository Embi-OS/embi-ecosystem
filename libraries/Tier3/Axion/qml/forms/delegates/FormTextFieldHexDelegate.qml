pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

FormObject {
    id: root

    formType: FormTypes.TextFieldHex
    valueType: FormValueTypes.Integer
    defaultValue: 0

    warning: !acceptableInput

    delegate: FormTextFieldHex {
        fitLabel: root.fitLabel
        labelWidthRatio: root.labelWidthRatio
        editable: root.editable

        enabled: root.enabled
        warning: root.warning || !acceptableInput

        validator: root.validator
        label: root.label
        infos: root.infos
        hex: root.currentValue

        onTextEdited: (text) => root.validateValue(text)
        onHexAccepted: (hex) => root.changeValue(hex)
    }
}
