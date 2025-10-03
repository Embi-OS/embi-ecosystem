pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

FormObject {
    id: root

    formType: FormTypes.TextFieldDate
    valueType: FormValueTypes.Date
    defaultValue: DateTimeUtils.invalidDate()
    validator: DateValidator {}

    warning: !acceptableInput

    delegate: FormTextFieldDate {
        fitLabel: root.fitLabel
        labelWidthRatio: root.labelWidthRatio
        editable: root.editable

        enabled: root.enabled
        warning: root.warning || !acceptableInput

        validator: root.validator
        label: root.label
        infos: root.infos
        date: root.currentValue

        onTextEdited: (text) => root.validateValue(text)
        onDateAccepted: (date) => root.changeValue(date)
    }
}
