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
    readonly property int columns: Math.max(1, Math.floor((width-root.columnSpacing)/(root.itemWidth+root.columnSpacing)))

    property bool selectable: false

    signal delegateClicked(int index)
    signal delegateLongPress(int index)

    cellWidth: count<=1 ? width : count<=columns ? width/count : width/columns
    cellHeight: root.itemHeight + root.rowSpacing

    topMargin: rowSpacing/2
    leftMargin: (cellWidth-itemWidth)/2
    rightMargin: -leftMargin
}
