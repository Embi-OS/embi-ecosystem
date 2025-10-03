import QtQuick
import Eco.Tier3.Axion

FormTextField {
    id: root

    signal hexAccepted(int hex)

    property int hex
    text: FormatUtils.hex(hex)

    property string displayMode: "hexadecimal"

    placeholder: qsTr("Valeur en hexadécimal")

    onAccepted: (text) => {
        root.hexAccepted(FormatUtils.intFromHex(text))
    }
}
