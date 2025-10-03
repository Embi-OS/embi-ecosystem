import QtQuick
import Eco.Tier3.Axion

FormTextField {
    id: root

    signal passwordAccepted(string password)

    property alias password: root.text

    echoMode: TextInput.Password
    placeholder: qsTr("Mot de passe")

    onAccepted: (text) => {
        root.passwordAccepted(text)
    }

    icons: [
        ClickableIcon {
            id: eyeIcon
            icon: MaterialIcons.eyeOutline

            pressedStateChanges: [
                PropertyChanges { root.echoMode: TextInput.Normal },
                PropertyChanges { eyeIcon.icon: MaterialIcons.eyeOffOutline }
            ]
        }
    ]
}
