import QtQuick
import Eco.Tier1.Utils
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

    function setSource(source: url, properties: var): var {
        return loader.setSource(source, properties)
    }

    function queueInvalidate() {
        loader.active = false
        if(delayed)
            throttler.throttle()
        else
            invalidate()
    }

    function invalidate() {
        throttler.cancel()
        loader.asynchronous = root.asynchronous
        loader.source = root.source
        loader.sourceComponent = root.delegate
        loader.active = root.active
    }

    SignalTrailingDebouncer {
        id: throttler
        timeout: 0
        onTriggered: root.invalidate()
    }

    Loader {
        id: loader
        anchors.fill: parent
        visible: status===Loader.Ready
        active: false

        onStatusChanged: {
            if (status===Loader.Ready) {
                root.loaded()
            }
        }
    }

    BasicBusyIndicator {
        anchors.centerIn: parent
        visible: root.loading
    }
}
