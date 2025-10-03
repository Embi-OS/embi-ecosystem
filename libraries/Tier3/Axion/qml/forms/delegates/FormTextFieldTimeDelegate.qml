pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

FormObject {
    id: root

    formType: FormTypes.TextFieldTime
    valueType: FormValueTypes.Time
    defaultValue: DateTimeUtils.invalidTime()
    validator: TimeValidator {}

    warning: !acceptableInput

    delegate: FormTextFieldTime {
        id: formTextFieldDate
        fitLabel: root.fitLabel
        labelWidthRatio: root.labelWidthRatio
        editable: root.editable

        enabled: root.enabled
        warning: root.warning || !acceptableInput

        validator: root.validator
        label: root.label
        infos: root.infos
        time: root.currentValue

        onTextEdited: (text) => root.validateValue(text)
        onTimeAccepted: (time) => root.changeValue(time)
    }
}
