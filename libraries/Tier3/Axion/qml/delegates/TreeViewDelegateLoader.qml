import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

Item {
    id: root

    property Component delegate: null
    property bool asynchronous: false
    property bool unfolded: false

    property TreeView treeView
    property var model
    property int index
    property bool expanded
    property int hasChildren
    property int depth
    property StandardObject qtObject

    readonly property Item item: loader.item as Item

    property bool inPool: false
    property bool isReady: false
    signal loaded()

    implicitWidth: Math.max(1, loader.implicitWidth)
    implicitHeight: Math.max(1, loader.implicitHeight)

    TableView.onPooled: root.inPool = true
    TableView.onReused: root.inPool = false

    Component.onCompleted: {
        if(treeView && unfolded)
            treeView.expand(index)
    }

    onUnfoldedChanged: {
        if(!treeView)
            return;
        if(unfolded)
            treeView.expand(index)
        else
            treeView.collapse(index)
    }

    function bindItemProperty(propertyName: string, bindingValue: var) {
        if (item.hasOwnProperty(propertyName)) {
            item[propertyName] = Qt.binding(bindingValue);
        }
    }

    onItemChanged: {
        if(!item)
            return
        bindItemProperty("treeView", () => treeView)
        bindItemProperty("model", () => model)
        bindItemProperty("index", () => index)
        bindItemProperty("expanded", () => expanded)
        bindItemProperty("hasChildren", () => hasChildren)
        bindItemProperty("depth", () => depth)
        bindItemProperty("qtObject", () => qtObject)

        isReady = true
        loaded()
    }

    Loader {
        id: loader
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: root.depth * 20
        active: true
        sourceComponent: root.delegate
        asynchronous: root.asynchronous
    }
}
