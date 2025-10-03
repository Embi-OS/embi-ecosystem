import QtQuick
import Eco.Tier3.Axion

Switch {
    id: root

    spacing: 8
    padding: 8

    font: Style.textTheme.button

    opacity: enabled ? 1.0 : 0.5
    Behavior on opacity { NumberAnimation { duration: 150 } }
}
