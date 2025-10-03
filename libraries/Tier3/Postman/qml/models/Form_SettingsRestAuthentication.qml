import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

FormObjectModel {
    id: root

    FormTextFieldDelegate {
        label: qsTr("Identifiant")
        placeholder: label
        targetProperty: "identifier"
    }
    FormTextFieldPasswordDelegate {
        label: qsTr("Mot de passe")
        targetProperty: "password"
    }
}
