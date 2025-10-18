import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import Eco.Tier3.Postman

FormObjectModel {
    id: root

    FormComboBoxDelegate {
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
        label: qsTr("URL")
        targetProperty: "apiBaseUrl"
        validator: UrlValidator{}
    }
    FormTextFieldDelegate {
        label: qsTr("Port")
        validator: SocketPortValidator{}
        valueType: FormValueTypes.Integer
        targetProperty: "apiPort"
    }
    FormSwitchDelegate {
        label: qsTr("Trailing slash")
        targetProperty: "apiTrailingSlash"
    }
}
