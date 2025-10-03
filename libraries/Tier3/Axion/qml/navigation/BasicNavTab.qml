import QtQuick
import Eco.Tier3.Axion

Item {
    id: root

    required property string stackItemName
    property string stackItemSecondaryName: ""

    property bool navigationLocked: false
    property bool navigationEnabled: true

    signal itemIsReady()

    property bool isCompleted: false
    property bool isReady: false
    Component.onCompleted: {
        Qt.callLater(itemIsReady);
        root.isCompleted=true
    }

    Component.onDestruction: {
        root.isReady=false
        root.isCompleted=false
    }

    onItemIsReady: {
        console.info("BasicNavTab active:",stackItemName,stackItemSecondaryName!==""?"-> \""+stackItemSecondaryName+"\"":"")
        root.isReady=true
    }
}
