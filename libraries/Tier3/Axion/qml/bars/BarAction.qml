pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion

ColumnContainer {
    id: root

    required property StandardObjectModel model

    backgroundVisible: visibleItems>0

    drawFrame: true
    color: Style.colorPrimaryDark

    Instantiator {
        onObjectAdded: (index, object) => root.addItem(object as Item)
        onObjectRemoved: (index, object) => root.removeItem(object as Item)

        model: ProxyModel {
            delayed: true
            sourceModel: root.model
            filterRoleName: "visible"
            filterValue: true
        }
        delegate: ButtonAction {
            required property StandardObject qtObject
            required visible
            required highlighted
            required enabled
            required text
            icon.source: qtObject?.icon ?? ""
            onClicked: qtObject.emitAction()
        }
    }
}
