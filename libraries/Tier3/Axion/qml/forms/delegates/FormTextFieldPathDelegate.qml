pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

FormObject {
    id: root

    formType: FormTypes.TextFieldPath
    defaultValue: ""

    warning: !acceptableInput
    placeholder: "Path/to/file"

    delegate: FormTextFieldPath {
        fitLabel: root.fitLabel
        labelWidthRatio: root.labelWidthRatio
        editable: root.editable

        enabled: root.enabled
        warning: root.warning || !acceptableInput

        validator: root.validator
        label: root.label
        infos: root.infos
        path: root.currentValue
        placeholder: root.placeholder

        onTextEdited: (text) => root.validateValue(text)
        onPathAccepted: (path) => root.changeValue(path)
    }
}
