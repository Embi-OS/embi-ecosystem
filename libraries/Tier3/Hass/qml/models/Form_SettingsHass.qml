import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import Eco.Tier3.Hass

FormObjectModel {
    id: root

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
    FormTextFieldPasswordDelegate {
        label: qsTr("Jeton d'accès longue durée")
        targetProperty: "apiToken"
    }
}
