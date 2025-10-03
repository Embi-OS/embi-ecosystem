pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion

BasicPane {
    id: root

    navigationLocked: currentItem?.navigationLocked ?? false
    navigationEnabled: loader.ready && (currentItem?.navigationEnabled ?? true)

    property alias proxyModel: proxyModel
    readonly property int currentIndex: proxyModel.count>0 ? proxyModel.mapToSource(tabBar.currentIndex) : -1
    required property StandardObjectModel tabsModel
    readonly property StandardObject currentObject: tabsModel.at(currentIndex) as StandardObject
    property bool asynchronous: currentObject?.asynchronous ?? false
    readonly property Component currentComponent: currentObject?.delegate ?? defaultComponent

    property Component defaultComponent: Placeholder {}
    readonly property Item currentItem: loader.item as Item
    property alias active: loader.active

    function setCurrentIndex(index: int) {
        tabBar.setCurrentIndex(index)
    }

    function decrementCurrentIndex() {
        tabBar.decrementCurrentIndex()
    }

    function incrementCurrentIndex() {
        tabBar.incrementCurrentIndex()
    }

    header: BasicTabBar {
        id: tabBar
        topInset: root.headerPadding
        leftInset: root.headerPadding
        rightInset: root.headerPadding

        opacity: enabled ? 1.0 : 0.5
        enabled: root.navigationEnabled && !root.navigationLocked
        visible: count>1

        Repeater {
            model: ProxyModel {
                id: proxyModel
                // enabled: root.isCompleted
                delayed: true
                sourceModel: root.tabsModel
                filterRoleName: "visible"
                filterValue: true
            }
            delegate: BasicTabButton {
                opacity: 1.0
                required property StandardObject qtObject
                required property int index
                required enabled
                required text
                icon.source: qtObject?.icon ?? ""
                onClicked: {
                    if(checked)
                        loader.queueInvalidate()
                }
            }
        }
    }

    contentItem: ViewDelegateLoader {
        id: loader

        delayed: true
        asynchronous: root.asynchronous
        delegate: root.currentComponent
        active: root.isCompleted
    }
}
