import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

BasicContainer {
    id: root

    readonly property int visibleItems: itemContainer.visibleCount

    property alias flow: layout.flow
    property alias layoutDirection: layout.layoutDirection
    property alias columns: layout.columns
    property alias rows: layout.rows
    property alias uniformCellHeights: layout.uniformCellHeights
    property alias uniformCellWidths: layout.uniformCellWidths
    property bool flickable: false
    property int flickableDirection: Flickable.AutoFlickIfNeeded

    property int orientation: Qt.Horizontal
    property int alignment: Qt.AlignVCenter | Qt.AlignHCenter
    property bool fillHeight: false
    property bool fillWidth: false

    signal itemAdded(index: int, item: Item)
    signal itemRemoved(index: int, item: Item)

    onItemAdded: (index, item) => {
        if(!item)
            return;

        itemContainer.addItem(item)

        if(item instanceof T.ToolSeparator) {
            if(root.orientation===Qt.Horizontal)
                item.orientation = Qt.Vertical
            else if(root.orientation===Qt.Vertical)
                item.orientation = Qt.Horizontal
        }

        item.Layout.alignment|=alignment

        if(fillHeight)
            item.Layout.fillHeight=true
        if(fillWidth)
            item.Layout.fillWidth=true
    }

    onItemRemoved: (index, item) => {
        itemContainer.removeItem(item)
    }

    ItemContainer {
        id: itemContainer
        delayed: true
    }

    contentItem: Flickable {
        id: flickable
        implicitHeight: layout.implicitHeight
        implicitWidth: layout.implicitWidth
        contentHeight: Math.max(height, layout.height)
        contentWidth: Math.max(width, layout.width)
        flickableDirection: root.flickableDirection
        clip: root.flickable

        GridLayout {
            id: layout

            height: root.flickable ? Math.max(flickable.height, layout.implicitHeight) : flickable.height
            width: root.flickable ? Math.max(flickable.width, layout.implicitWidth) : flickable.width

            rowSpacing: root.spacing
            columnSpacing: root.spacing

            Repeater {
                model: root.contentModel
                onItemAdded: (index, item) => {
                    root.itemAdded(index, item)
                }
                onItemRemoved: (index, item) => {
                    root.itemRemoved(index, item)
                }
            }
        }
    }
}
