pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

GridView {
    id: root

    clip: true
    flickableDirection: Flickable.VerticalFlick
    interactive: true
    reuseItems: true

    property int rowSpacing: 10
    property int columnSpacing: 10
    property int itemWidth: 500
    property int itemHeight: 200
    readonly property int columns: count<Math.floor(width/(root.itemWidth+root.columnSpacing)) ? count : Math.floor(width/(root.itemWidth+root.columnSpacing))

    cellWidth: width/root.columns
    cellHeight: root.itemHeight + root.rowSpacing

    property Component itemDelegate
    delegate: Item {
        required property var model
        required property int index
        readonly property Item item: loader.item as Item

        implicitWidth: root.cellWidth
        implicitHeight: root.cellHeight

        Component.onCompleted: loader.active=true
        Component.onDestruction: loader.active=false
        // GridView.onAdd: loader.active=true
        // GridView.onPooled: loader.active=false
        // GridView.onRemove: loader.active=false
        // GridView.onReused: loader.active=true

        function bindItemProperty(propertyName: string, bindingValue: var) {
            if (item.hasOwnProperty(propertyName)) {
                item[propertyName] = Qt.binding(bindingValue);
            }
        }

        onItemChanged: {
            if(!item)
                return
            bindItemProperty("model", () => model)
            bindItemProperty("index", () => index)
        }

        Loader {
            id: loader
            anchors.centerIn: parent
            active: false
            sourceComponent: root.itemDelegate
        }
    }
}
