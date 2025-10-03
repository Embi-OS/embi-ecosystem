import QtQuick
import Eco.Tier3.Axion

Item {
    id: root

    visible: loader.status===Loader.Ready

    signal loaded()
    property bool active: true
    property int delay: 0

    property Component sourceComponent
    property string sourceModule
    property string sourceName
    property alias asynchronous: loader.asynchronous
    property alias item: loader.item
    property alias status: loader.status

    onActiveChanged: Qt.callLater(process)
    Component.onCompleted: Qt.callLater(process)

    Loader {
        id: loader
        anchors.fill: parent
        asynchronous: true
        active: false

        onStatusChanged: {
            if (status!==Loader.Ready)
                return;

            Log.debug(("load %1 took: %2 ms").arg(root.sourceName).arg(Log.elapsed()))

            timer.start()
        }
    }

    Timer {
        id: timer
        interval: root.delay
        repeat: false
        onTriggered: {
            loader.visible = true;
            if(loader.item as Item) {
                (loader.item as Item).visible = true;
            }
            root.loaded()
        }
    }

    function process() {
        if (root.active)
            show();
        else
            hide();
    }

    function show() {
        timer.stop()
        Log.startElapsed()
        if(root.sourceComponent) {
            loader.sourceComponent = root.sourceComponent;
        }
        else {
            loader.sourceComponent = Qt.createComponent(sourceModule, sourceName, root.asynchronous ? Component.Asynchronous : Component.PreferSynchronous, root);
            Log.debug(("createComponent %1 took: %2 ms").arg(root.sourceName).arg(Log.elapsed()))
        }
        loader.active = true
    }

    function hide() {
        timer.stop()
        loader.active = false
        loader.sourceComponent = undefined;
        loader.visible = false;
    }
}
