import QtQuick
import Eco.Tier3.Axion

Item {
    id: root

    Navigation.name: ""
    Navigation.secondaryName: ""

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
        root.isReady=true
    }
}
