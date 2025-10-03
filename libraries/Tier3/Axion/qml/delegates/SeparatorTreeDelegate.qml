pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

StandardObject {
    id: root

    highlighted: false

    delegate: BasicSeparator {
        highlighted: root.highlighted
        orientation: Qt.Horizontal
    }
}
