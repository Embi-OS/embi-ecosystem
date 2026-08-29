import QtQuick
import Eco.Tier3.Axion

Item {
    id: root

    clip: true

    property alias item: loader.item
    property alias status: loader.status
    property alias progress: loader.progress

    readonly property bool swipeViewActive: SwipeView.isCurrentItem && (SwipeView.view?.visible ?? false)
    readonly property bool stackLayoutActive: StackLayout.isCurrentItem && (StackLayout.layout?.visible ?? false)
    readonly property bool stackViewActive: StackView.index>=0 && (StackView.view?.visible ?? false)
    readonly property bool viewActive: root.visible && (root.swipeViewActive || root.stackLayoutActive || root.stackViewActive)

    readonly property bool loading: loader.status===Loader.Loading || !loader.active
    readonly property bool ready: loader.status===Loader.Ready

    property bool delayed: true
    property bool asynchronous: true
    property string source: ""
    default property Component delegate: null
    property bool active: viewActive

    onAsynchronousChanged: queueInvalidate()
    onSourceChanged: queueInvalidate()
    onDelegateChanged: queueInvalidate()
    onActiveChanged: queueInvalidate()
    Component.onCompleted: queueInvalidate()

    signal loaded()
    signal error()

    onError: {
        AxionHelper.warningNavigation(("Unable to load %1").arg(root.delegate));
    }

    function setSource(source: url, properties: var): var {
        return loader.setSource(source, properties)
    }

    function queueInvalidate() {
        loader.active = false
        if(delayed)
            throttler.start()
        else
            invalidate()
    }

    function invalidate() {
        throttler.stop()

        if(root.active)
            Log.startElapsed(("loader_%1").arg(root))

        loader.asynchronous = root.asynchronous
        loader.source = root.source
        loader.sourceComponent = root.delegate
        loader.active = root.active
    }

    Timer {
        id: throttler
        interval: 0
        onTriggered: root.invalidate()
    }

    Loader {
        id: loader
        anchors.fill: parent
        visible: status===Loader.Ready
        active: false

        onStatusChanged: {
            const key = ("loader_%1").arg(root)

            if (status===Loader.Ready) {
                const elapsed = Log.endElapsed(key)
                if(elapsed>=0)
                    Log.trace(("load %1 took: %2 ms").arg(FormatUtils.qmlTypeName(item)).arg(elapsed))
                root.loaded()
            }
            if (status===Loader.Error) {
                Log.endElapsed(key)
                root.error()
            }
        }
    }

    BasicBusyIndicator {
        anchors.centerIn: parent
        visible: root.loading
    }
}
