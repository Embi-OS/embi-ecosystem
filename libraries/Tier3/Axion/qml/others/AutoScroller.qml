import QtQuick
import QtQuick.VirtualKeyboard
import Eco.Tier3.Axion

QtObject {
    id: root

    property bool enabled: true
    property list<Flickable> flickables: []
    readonly property Flickable flickable: flickables.length>0 ? flickables[0] : null
    readonly property Item inputItem: InputContext.priv.inputItem as Item

    readonly property BasicWindow rootWindow: flickable?.Window.window as BasicWindow ?? null
    readonly property double windowHeight: rootWindow?.contentHeight ?? rootWindow?.height ?? 0
    readonly property double keyboardHeight: rootWindow?.keyboardHeight ?? 0
    readonly property double flickableBottomPositionY: flickable ? flickable.ScenePosition.y+flickable.height : 0
    readonly property bool keyboardVisible: Qt.inputMethod && Qt.inputMethod.visible
    readonly property double contentBottomMargin: keyboardVisible ? Math.max((keyboardHeight+scrollMargin)-(windowHeight-flickableBottomPositionY), 0) : 0
    property double scrollMargin: Style.contentRectangleRadius

    onInputItemChanged: process()
    onEnabledChanged: process()

    function process() {
        root.flickables = []
        if (root.inputItem !== null && root.enabled) {
            var candidate = root.inputItem.parent
            while (candidate) {
                var flickable = (candidate as Flickable)
                if (flickable && flickable.maximumFlickVelocity) {
                    root.flickables.push(flickable)
                }
                candidate = candidate.parent
            }
            if(root.flickables.length>0)
                delayedLoading.restart()
        }
    }

    function ensureVisible(flickable: Flickable) {
        if(!flickable || !root.enabled)
            return

        if (root.inputItem && flickable && flickable.visible && flickable.interactive) {

            var verticallyFlickable = (flickable.flickableDirection === Flickable.HorizontalAndVerticalFlick || flickable.flickableDirection === Flickable.VerticalFlick
                                       || (flickable.flickableDirection === Flickable.AutoFlickDirection && flickable.contentHeight > (flickable.height - flickable.topMargin - flickable.bottomMargin))
                                       || (flickable.flickableDirection === Flickable.AutoFlickIfNeeded && flickable.contentHeight > (flickable.height - flickable.topMargin - flickable.bottomMargin)))
            var horizontallyFlickable = (flickable.flickableDirection === Flickable.HorizontalAndVerticalFlick || flickable.flickableDirection === Flickable.HorizontalFlick
                                         || (flickable.flickableDirection === Flickable.AutoFlickDirection && flickable.contentWidth > (flickable.width - flickable.leftMargin - flickable.rightMargin))
                                         || (flickable.flickableDirection === Flickable.AutoFlickIfNeeded && flickable.contentWidth > (flickable.width - flickable.leftMargin - flickable.rightMargin)))

            if ((!verticallyFlickable && !horizontallyFlickable) || !inputItem.hasOwnProperty("cursorRectangle"))
                return

            var cursorRectangle = flickable.contentItem.mapFromItem(inputItem, inputItem.cursorRectangle.x, inputItem.cursorRectangle.y)

            if (verticallyFlickable) {
                if (flickable.contentY - flickable.topMargin >= cursorRectangle.y - scrollMargin) {
                    flickable.contentY = Math.max(0, cursorRectangle.y - scrollMargin) + flickable.topMargin
                }
                else if (flickable.contentY + flickable.height - flickable.topMargin - flickable.bottomMargin <= cursorRectangle.y + inputItem.cursorRectangle.height + scrollMargin) {
                    flickable.contentY = Math.min(flickable.contentHeight - flickable.height, cursorRectangle.y + inputItem.cursorRectangle.height - flickable.height + scrollMargin) + flickable.bottomMargin
                }
            }
            if (horizontallyFlickable) {
                if (flickable.contentX - flickable.leftMargin >= cursorRectangle.x - scrollMargin) {
                    flickable.contentX = Math.max(0, cursorRectangle.x - scrollMargin) + flickable.leftMargin
                }
                else if (flickable.contentX + flickable.width - flickable.leftMargin - flickable.rightMargin <= cursorRectangle.x + inputItem.cursorRectangle.width + scrollMargin) {
                    flickable.contentX = Math.min(flickable.contentWidth - flickable.width, cursorRectangle.x + inputItem.cursorRectangle.width - flickable.width + scrollMargin) + flickable.rightMargin
                }
            }
        }
    }

    readonly property Binding binding: Binding {
        when: root.enabled
        target: root.flickable
        property: "bottomMargin"
        value: root.contentBottomMargin
    }

    readonly property Timer timer: Timer {
        id: delayedLoading
        interval: 0
        onTriggered: {
            if(!root.enabled)
                return
            for(const flickable of root.flickables)
                root.ensureVisible(flickable)
        }
    }

    readonly property Connections connections: Connections {
        enabled: root.enabled && root.inputItem
        ignoreUnknownSignals: true
        target: Qt.inputMethod
        function onCursorRectangleChanged() {
            if (root.inputItem && root.inputItem.activeFocus)
                delayedLoading.restart()
        }
    }
}
