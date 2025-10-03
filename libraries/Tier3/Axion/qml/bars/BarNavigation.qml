pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.ProxyModel
import Eco.Tier1.Models
import Eco.Tier3.Axion

RowLayout {
    id: root

    required property StandardObjectModel model
    required property int position
    readonly property bool firstRow: !position
    property bool navigationLocked: false

    signal clicked(int navIndex)

    spacing: -2*Style.barNavigationBorderRadius-2*Style.barNavigationBorderWidth

    Repeater {
        id: repeater
        model: ProxyModel {
            delayed: true
            sourceModel: root.model
            filterRoleName: "visible"
            filterValue: true
        }
        delegate: ButtonNavigation {
            navigationLocked: root.navigationLocked
            required property StandardObject qtObject
            required property int index
            required highlighted
            required icon
            required enabled
            required text
            z: -index

            position: index
            isFirstRow: root.firstRow

            onClicked: (position) => {
                qtObject.emitAction()
                if(!highlighted)
                    root.clicked(position)
            }
        }
    }
}
