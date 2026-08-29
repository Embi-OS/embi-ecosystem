import QtQuick
import Eco.Tier3.Axion

BasicNavItem {
    id: root

    property string stackBranchName: ""

    required property BasicNavObject initialObject
    readonly property BasicNavStack navStack: navStack
    readonly property BasicNavItem currentItem: navStack.currentItem as BasicNavItem

    Navigation.guarded: currentItem?.Navigation.guarded ?? false
    Navigation.name: currentItem?.Navigation.name ?? ""
    Navigation.effectiveName: (currentItem?.Navigation.effectiveName ?? "" !== "") ? currentItem.Navigation.effectiveName :
                              (currentItem?.Navigation.name ?? "" !== "") ? currentItem.Navigation.name :
                              root.Navigation.name
    Navigation.secondaryName: currentItem?.Navigation.secondaryName ?? ""
    navModels: currentItem?.navModels ?? null
    state: currentItem?.state ?? ""

    BasicNavStack {
        id: navStack
        anchors.fill: parent
        initialObject: root.initialObject
    }
}
