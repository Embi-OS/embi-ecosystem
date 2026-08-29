import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

FormObjectModel {
    FormTextFieldDelegate {
        label: qsTr("Nom")
        placeholder: qsTr("Max. 128 caractères")
        validator: StringValidator {
            minSize: 1
            maxSize: 128
        }
        targetProperty: "name"
    }
}
