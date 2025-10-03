pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

FormObject {
    id: root

    formType: FormTypes.TextField
    defaultValue: ""

    property int textType: FormTextTypes.String
    property int inputMethodHints: valueType===FormValueTypes.Integer || valueType===FormValueTypes.Double ? Qt.ImhFormattedNumbersOnly : 0
    property int echoMode: TextInput.Normal
    readonly property string currentText: currentValue

    warning: !acceptableInput

    delegate: FormTextField {
        fitLabel: root.fitLabel
        labelWidthRatio: root.labelWidthRatio
        editable: root.editable

        enabled: root.enabled
        warning: root.warning || !acceptableInput

        validator: root.validator
        label: root.label
        infos: root.infos
        placeholder: root.placeholder
        text: root.currentValue

        textType: root.textType
        inputMethodHints: root.inputMethodHints
        echoMode: root.echoMode

        onTextEdited: (text) => root.validateValue(text)
        onAccepted: (text) => root.changeValue(text)
    }
}
