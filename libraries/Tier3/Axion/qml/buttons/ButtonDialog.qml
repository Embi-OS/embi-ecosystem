import QtQuick
import Eco.Tier3.Axion

TextButton {
    id: root

    preferredWidth: 150
    implicitWidth: visible ? Math.max(buttonImplicitWidth, preferredWidth) : 0

    color: Style.colorWhite
}
