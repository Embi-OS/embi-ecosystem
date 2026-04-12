import QtQuick
import Eco.Tier3.Axion

BasicNavItem {
    id: root

    property string stackBranchName: ""
    property string stackBranchSecondaryName: ""

    required property BasicNavObject initialObject
    readonly property BasicNavStack navStack: navStack
    readonly property BasicNavItem currentItem: navStack.currentItem as BasicNavItem

    stackItemName: currentItem?.stackItemName ?? ""
    stackItemSecondaryName: currentItem?.stackItemSecondaryName ?? ""
    navigationLocked: currentItem?.navigationLocked ?? false
    navigationEnabled: currentItem?.navigationEnabled ?? true
    navModels: currentItem?.navModels ?? null
    state: currentItem?.state ?? ""

    onItemIsActive: {
        console.info("BasicNavBranch active:",stackBranchName,stackBranchSecondaryName!==""?"("+stackBranchSecondaryName+")":"")
    }

    BasicNavStack {
        id: navStack
        anchors.fill: parent
        initialObject: root.initialObject
    }
}
