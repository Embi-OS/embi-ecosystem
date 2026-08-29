import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import Eco.Tier3.Postman

FormObjectModel {
    id: root

    FormSwitchDelegate {
        id: localApiEnabled
        label: qsTr("API interne")
        targetProperty: "localApiEnabled"
    }

    FormComboBoxDelegate {
        visible: !localApiEnabled.checked
        label: qsTr("Data mode")
        options: StandardObjectModel {
            StandardObject {text: "Json"; value: RestDataModes.Json}
            StandardObject {text: "Cbor"; value: RestDataModes.Cbor}
        }
        textRole: "text"
        valueRole: "value"
        targetProperty: "apiDataMode"
    }
    FormTextFieldDelegate {
        visible: !localApiEnabled.checked
        label: qsTr("URL")
        targetProperty: "apiBaseUrl"
        validator: UrlValidator{}
    }
    FormTextFieldDelegate {
        visible: !localApiEnabled.checked
        label: qsTr("Port")
        validator: SocketPortValidator{}
        valueType: FormValueTypes.Integer
        targetProperty: "apiPort"
    }
    FormSwitchDelegate {
        visible: !localApiEnabled.checked
        label: qsTr("Trailing slash")
        targetProperty: "apiTrailingSlash"
    }
    FormSwitchDelegate {
        visible: !localApiEnabled.checked
        label: qsTr("WebSocket")
        targetProperty: "apiSocketEnabled"
    }
}
