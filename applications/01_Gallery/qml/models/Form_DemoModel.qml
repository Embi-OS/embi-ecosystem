import QtQuick
import Eco.Tier1.Models
import Eco.Tier2.Unit
import Eco.Tier3.Axion

FormObjectModel {
    FormTextFieldDelegate {
        targetProperty: "textField"
        label: "TextField"
        placeholder: "Placeholder"
    }
    FormTextAreaDelegate {
        targetProperty: "textArea"
        label: "TextArea"
        placeholder: "Placeholder"
    }
    FormTextFieldDateDelegate {
        targetProperty: "textFieldDate"
        label: "TextFieldDate"
    }
    FormTextFieldTimeDelegate {
        targetProperty: "textFieldTime"
        label: "TextFieldTime"
    }
    FormTextFieldPathDelegate {
        targetProperty: "textFieldPath"
        label: "TextFieldPath"
        placeholder: "/Path/to/folder"
    }
    FormTextFieldHexDelegate {
        label: "TextFieldHex"
        placeholder: "0x7FFF"
        validator: HexValidator {
            bottom: 0x0000
            top: 0x7FFF
        }
        targetProperty: "textFieldHex"
    }
    FormTextFieldDelegate {
        targetProperty: "ipv4Address"
        label: "IPv4"
        validator: Ipv4Validator{}
    }
    FormTextFieldDelegate {
        targetProperty: "socketPort"
        label: "Socket Port"
        validator: SocketPortValidator{}
        valueType: FormValueTypes.Integer
    }
    FormNestDelegate {
        text: "Sous-titre"

        FormTextFieldUnitDelegate {
            label: "TextFieldUnit 2"
            placeholder: "Placeholder"
            unitOptions: Units.torqueUnitModel
            valueTargetProperty: "textFieldUnitValue"
            typeTargetProperty: "unitTextFieldUnitType"
        }
        FormTextFieldPasswordDelegate {
            targetProperty: "password"
            label: "Password"
            placeholder: "Placeholder"
        }
        FormSwitchDelegate {
            targetProperty: "switchVal"
            label: "Switch"
            leftLabel: "Left"
            rightLabel: "Right"
        }
        FormComboBoxDelegate {
            targetProperty: "dropDownList"
            label: "ComboBox"
            options: ListModel {
                ListElement {name: "Value 0"; value: 0}
                ListElement {name: "Value 1"; value: 1}
                ListElement {name: "Value 2"; value: 2}
                ListElement {name: "Value 3"; value: 3}
            }
            textRole: "name"
            valueRole: "value"
        }
    }
}
