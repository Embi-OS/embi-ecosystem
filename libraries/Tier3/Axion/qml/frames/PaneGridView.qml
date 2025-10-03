pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

BasicPane {
    id: root

    property AbstractItemModel model
    property alias itemDelegate: view.itemDelegate
    property alias view: view

    property alias rowSpacing: view.rowSpacing
    property alias columnSpacing: view.columnSpacing
    property alias itemWidth: view.itemWidth
    property alias itemHeight: view.itemHeight
    property alias reuseItems: view.reuseItems

    property alias viewFooter: view.footer
    property alias viewFooterItem: view.footerItem

    property alias viewHeader: view.header
    property alias viewHeaderItem: view.headerItem

    property bool canRefresh: false

    signal refreshTriggered()

    contentItem: BasicGridView {
        id: view
        implicitHeight: contentHeight
        implicitWidth: contentWidth

        model: root.model
        rowSpacing: 50
        columnSpacing: 50

        Loader {
            id: busyIndicatorLoader
            active: root.canRefresh
            sourceComponent: PullToRefreshHandle {
                parent: view
                onTriggered: root.refreshTriggered()
            }
        }
    }
}
