pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

Item {
    id: root

    clip: true

    property double spacing: 5

    property alias interactive: view.interactive
    property alias currentIndex: view.currentIndex
    readonly property Item currentItem: view.itemAt(currentIndex)

    default property alias views: view.contentData

    function addItem(item: Item){
        return view.addItem(item)
    }

    function decrementCurrentIndex() {
        view.decrementCurrentIndex()
    }

    function incrementCurrentIndex() {
        view.incrementCurrentIndex()
    }

    function insertItem(index: int, item: Item) {
        view.insertItem(index, item)
    }

    function itemAt(index: int): Item {
        view.itemAt(index)
    }

    function moveItem(from: int, to: int) {
        view.moveItem(from, to)
    }

    function removeItem(item: Item) {
        view.removeItem(item)
    }

    function setCurrentIndex(index: int) {
        view.setCurrentIndex(index)
    }

    function takeItem(index: int): Item {
        return view.takeItem(index)
    }

    BasicTabBar {
        id: tabBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        currentIndex: view.currentIndex

        Repeater {
            model: view.count
            delegate: BasicTabButton {
                width: visible ? undefined : -tabBar.spacing
                height: ListView.view?.height ?? undefined
                opacity: 1.0
                required property int index
                readonly property StandardObject qtObject: view.itemAt(index).StandardObject
                visible: qtObject.visible
                enabled: qtObject.enabled
                text: qtObject.text
                icon.source: qtObject.icon
            }
        }
    }

    SwipeView {
        id: view
        anchors.top: tabBar.bottom
        anchors.topMargin: root.spacing
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        currentIndex: tabBar.currentIndex
        interactive: false

        contentItem: ListView {
            model: view.contentModel
            interactive: view.interactive
            currentIndex: view.currentIndex
            focus: view.focus

            spacing: view.spacing
            orientation: view.orientation
            snapMode: ListView.SnapOneItem
            boundsBehavior: Flickable.StopAtBounds

            highlightRangeMode: ListView.StrictlyEnforceRange
            preferredHighlightBegin: 0
            preferredHighlightEnd: 0
            highlightMoveDuration: root.interactive ? 250 : 0
            maximumFlickVelocity: 4 * (view.orientation === Qt.Horizontal ? width : height)
        }
    }
}
