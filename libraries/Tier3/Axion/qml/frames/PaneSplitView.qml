pragma ComponentBehavior: Bound
import QtQuick
import Qt.labs.qmlmodels
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion

BasicPane {
    id: root

    navigationLocked: currentItem?.navigationLocked ?? false
    navigationEnabled: loader.ready && (currentItem?.navigationEnabled ?? true)

    readonly property int currentIndex: proxyModel.count>0 ? proxyModel.mapToSource(listView.currentIndex) : -1
    required property StandardObjectModel model
    readonly property StandardObject currentObject: model ? model.at(currentIndex) as StandardObject : null
    property bool asynchronous: currentObject?.asynchronous ?? false
    property Component currentComponent: currentObject?.delegate ?? defaultComponent

    property alias proxyModel: proxyModel
    property alias listView: listView
    property alias listDelegate: listView.delegate
    property alias splitView: splitView
    property alias buttonGroup: buttonGroup
    property alias itemContainer: itemContainer

    property Component defaultComponent: Placeholder {}
    readonly property Item currentItem: loader.item as Item
    property alias active: loader.active

    function setCurrentIndex(index: int) {
        splitView.setCurrentIndex(index)
    }

    function decrementCurrentIndex() {
        splitView.decrementCurrentIndex()
    }

    function incrementCurrentIndex() {
        splitView.incrementCurrentIndex()
    }

    ButtonGroup {
        id: buttonGroup
    }

    ItemContainer {
        id: itemContainer
        delayed: true
        enabledWidth: true
    }

    contentItem: BasicSplitView {
        id: splitView
        BasicListView {
            id: listView

            SplitView.fillHeight: true
            SplitView.maximumWidth: parent.width/2
            SplitView.minimumWidth: 48 + verticalScrollerWidth
            SplitView.preferredWidth: itemContainer.maxImplicitWidth + verticalScrollerWidth

            opacity: enabled ? 1.0 : 0.5
            enabled: root.navigationEnabled && !root.navigationLocked
            spacing: 0

            section.property: "group"
            section.delegate: BasicSeparator {
                width: (ListView.view as BasicListView).viewWidth
                orientation: Qt.Horizontal
            }

            model: ProxyModel {
                id: proxyModel
                // enabled: root.isCompleted
                delayed: true
                sourceModel: root.model
                filterRoleName: "visible"
                filterValue: true
                sortRoleName: "group"
                sortOrder: Qt.AscendingOrder
            }
            delegate: IconTitleDelegate {
                id: iconTitle
                ButtonGroup.group: buttonGroup
                ItemContainer.container: itemContainer
                opacity: 1.0
                width: (ListView.view as BasicListView).viewWidth
                required property StandardObject qtObject
                required property StandardObjectModel value
                required property int index
                required enabled
                required text
                required hint
                icon.source: qtObject?.icon ?? ""

                checkable: true
                highlighted: ListView.isCurrentItem
                onClicked: {
                    if(ListView.isCurrentItem)
                        loader.queueInvalidate()
                    else
                        root.setCurrentIndex(index)
                }

                showIndicator: childrenFiltered.count>1
                ModelMatcher {
                    id: childrenFiltered
                    sourceModel: iconTitle.value
                    roleName: "visible"
                    value: true
                    delayed: true
                    hits: 2
                }
            }

            ScrollIndicator.vertical: BasicScrollIndicator {
                alwaysOn: true
                visible: listView.contentHeight>listView.height
            }

            currentIndex: splitView.currentIndex
            highlightMoveDuration: 250
            highlightResizeDuration: 0
            highlightFollowsCurrentItem: true

            highlight: Item {
                z: -1
                Rectangle {
                    anchors.centerIn: parent
                    height: parent.height
                    width: parent.width
                    radius: Style.itemDelegateRadius
                    color: Style.colorTransparent
                    border.color: Style.colorAccent
                    border.width: Style.sideButtonBorderWidth
                }
            }
        }

        ViewDelegateLoader {
            id: loader
            SplitView.fillHeight: true
            SplitView.fillWidth: true

            delayed: true
            asynchronous: root.asynchronous
            delegate: root.currentComponent
            active: root.isCompleted
        }
    }
}
