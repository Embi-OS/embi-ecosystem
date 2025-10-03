pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

FormObject {
    id: root

    formType: FormTypes.TextFieldPassword
    defaultValue: ""

    warning: !acceptableInput

    delegate: FormTextFieldPassword {
        fitLabel: root.fitLabel
        labelWidthRatio: root.labelWidthRatio
        editable: root.editable

        enabled: root.enabled
        warning: root.warning || !acceptableInput

        validator: root.validator
        label: root.label
        infos: root.infos
        password: root.currentValue

        onTextEdited: (text) => root.validateValue(text)
        onPasswordAccepted: (password) => root.changeValue(password)
    }
}
