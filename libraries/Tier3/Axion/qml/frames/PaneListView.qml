import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

BasicPane {
    id: root

    property AbstractItemModel model
    property alias delegate: view.delegate
    property alias view: view

    property alias viewWidth: view.viewWidth
    property alias viewHeight: view.viewHeight
    property alias reuseItems: view.reuseItems
    property alias viewSection: view.section
    property alias viewCount: view.count

    property alias viewFooter: view.footer
    property alias viewFooterItem: view.footerItem
    property alias viewFooterPositioning: view.footerPositioning

    property alias viewHeader: view.header
    property alias viewHeaderItem: view.headerItem
    property alias viewHeaderPositioning: view.headerPositioning

    property bool transitionsEnabled: true
    property double transitionsDuration: 150
    property alias viewPopulate: view.populate
    property alias viewAdd: view.add
    property alias viewRemove: view.remove
    property alias viewMove: view.move
    property alias viewDisplaced: view.displaced
    property alias viewAddDisplaced: view.addDisplaced
    property alias viewRemoveDisplaced: view.removeDisplaced
    property alias viewMoveDisplaced: view.moveDisplaced

    property bool canRefresh: false

    signal refreshTriggered()

    function gotoIndex(index: int, mode: int) {
        view.gotoIndex(index, mode)
    }

    function gotoSubItem(item: Item, mode: int) {
        view.gotoSubItem(item, mode)
    }

    contentItem: BasicListView {
        id: view

        model: root.model
        spacing: root.spacing

        footerPositioning: ListView.OverlayFooter
        headerPositioning: ListView.OverlayHeader

        ScrollBar.vertical: BasicScrollBar {
            visible: view.contentHeight>view.height
        }

        add: Transition {
            enabled: root.transitionsEnabled
            NumberAnimation { property: "opacity"; from: 0; to: 1; duration: root.transitionsDuration }
            NumberAnimation { property: "scale"; from: 0.8; to: 1; duration: root.transitionsDuration }
        }

        move: Transition {
            enabled: root.transitionsEnabled
            NumberAnimation { properties: "x,y"; duration: root.transitionsDuration }

            // ensure opacity and scale values return to 1.0
            NumberAnimation { property: "opacity"; to: 1.0 }
            NumberAnimation { property: "scale"; to: 1.0 }
        }

        displaced: Transition {
            enabled: root.transitionsEnabled
            NumberAnimation { properties: "x,y"; duration: root.transitionsDuration }

            // ensure opacity and scale values return to 1.0
            NumberAnimation { property: "opacity"; to: 1.0 }
            NumberAnimation { property: "scale"; to: 1.0 }
        }

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
