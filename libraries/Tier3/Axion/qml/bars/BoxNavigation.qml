pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

ColumnLayout {
    id: root

    signal clicked(int barIndex, int navIndex)

    required property StandardObjectModel model
    property bool navigationLocked: false

    spacing: -Style.barNavigationBorderWidth
    x: -Style.barNavigationBorderRadius
    y: -Style.barNavigationBorderRadius

    Repeater {
        id: repeater
        model: root.model
        delegate: BarNavigation {
            navigationLocked: root.navigationLocked
            required property StandardObject qtObject
            required property StandardObjectModel value
            required property int index
            required visible
            model: value
            position: index

            onClicked: (navIndex) => root.clicked(position, navIndex)
        }
    }
}
