import QtQuick
import Eco.Tier3.Axion

import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Settings

Item {
    id: root

    width: 800
    height: 500

    ListModel {
        id: listModel
        ListElement { text: "Value 1"; value: 1 }
        ListElement { text: "Value 2"; value: 2 }
        ListElement { text: "Value 3"; value: 3 }
        ListElement { text: "Value 4"; value: 4 }
        ListElement { text: "Value 5"; value: 5 }
        ListElement { text: "Value 6"; value: 6 }
        ListElement { text: "Value 7"; value: 7 }
        ListElement { text: "Value 8"; value: 8 }
        ListElement { text: "Value 9"; value: 9 }
    }

    ColumnContainer {
        fillWidth: true
        flickable: true
        anchors.fill: parent

        Label {
            color: "#565758"
            text: "Combo field"
            font.pixelSize: 22
        }
        FormComboCheckList {
            label: "gdgs"
            options: listModel
            textRole: "text"
            valueRole: "value"
            Component.onCompleted: currentIndexes = [1,2,5]
        }
        FormComboBox {
            label: "gdgs"
            options: listModel
            textRole: "text"
            valueRole: "value"
            currentIndex: 3
        }

        Label {
            color: "#565758"
            text: "Tap fields to enter text"
            font.pixelSize: 22
        }
        FormTextField {
            label: "gdgs"
            placeholder: "One line field"
            onAccepted: passwordField.focus = true
        }
        FormTextField {
            id: passwordField
            label: "gdgs"
            echoMode: TextInput.Password
            placeholder: "Password field"
            inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhPreferLowercase | Qt.ImhSensitiveData | Qt.ImhNoPredictiveText
            //onAccepted: upperCaseField.focus = true
        }
        FormTextField {
            id: upperCaseField
            label: "gdgs"
            placeholder: "Upper case field"
            inputMethodHints: Qt.ImhUppercaseOnly
            //onAccepted: lowerCaseField.focus = true
        }
        FormTextField {
            id: lowerCaseField
            editable: false
            label: "gdgs"
            text: "foihgo"
            placeholder: "Lower case field"
            inputMethodHints: Qt.ImhLowercaseOnly
            //onAccepted: phoneNumberField.focus = true
        }
        FormTextFieldPassword {
            id: phoneNumberField
            editable: false
            label: "gdgs"
            placeholder: "Password"
            //onAccepted: formattedNumberField.focus = true
        }
        FormTextField {
            id: formattedNumberField
            label: "gdgs"
            placeholder: "Formatted number field"
            inputMethodHints: Qt.ImhFormattedNumbersOnly
            //onAccepted: digitsField.focus = true
        }
        FormTextFieldUnit {
            label: "gdgs"
            placeholder: "Digits only field"
            //onAccepted: textArea.focus = true
        }
        FormSwitch {
            label: "gdgs"
            placeholder: "Digits only field"
            //onAccepted: textArea.focus = true
        }
        FormTextField {
            id: digitsField
            label: "gdgs"
            placeholder: "Digits only field"
            inputMethodHints: Qt.ImhDigitsOnly
            //onAccepted: textArea.focus = true
        }
        FormTextArea {
            id: textArea
            label: "gdgs"
            placeholder: "Multiple line field"
            height: Math.max(206, implicitHeight)
        }
    }
}
