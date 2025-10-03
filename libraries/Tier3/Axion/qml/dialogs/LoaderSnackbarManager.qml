pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion

Item {
    id: root

    parent: Overlay.overlay
    z: Infinity

    implicitHeight: Math.min(windowHeight, listView.contentHeight)
    implicitWidth: Math.min(windowWidth, Style.snackbarWidth)

    visible: !isEmpty

    property int position: ItemPositions.Top
    property double margins: 0
    property double xMargin: margins
    property double yMargin: margins

    property double elevation: Style.snackbarElevation
    property bool filled: false
    property bool outlined: false

    readonly property bool isEmpty: SnackbarManager.children.isEmpty
    readonly property BasicWindow rootWindow: Window.window as BasicWindow
    readonly property double windowWidth: rootWindow?.contentWidth ?? rootWindow?.width ?? 0
    readonly property double windowHeight: rootWindow?.contentHeight ?? rootWindow?.height ?? 0

    property bool transitionsEnabled: true
    property double transitionsDuration: 150

    x: {
        if (!parent)
            return 0
        switch (position) {
        case ItemPositions.TopLeft:
        case ItemPositions.Left:
        case ItemPositions.BottomLeft:
            return 0 + xMargin
        case ItemPositions.Top:
        case ItemPositions.Bottom:
            return (parent.width - width) / 2 + xMargin
        case ItemPositions.TopRight:
        case ItemPositions.Right:
        case ItemPositions.BottomRight:
            return parent.width - width + xMargin
        }
        console.warn("Unknown Position:", position)
        return (parent.width - width) / 2
    }

    y: {
        if (!parent)
            return 0
        switch (position) {
        case ItemPositions.TopLeft:
        case ItemPositions.Top:
        case ItemPositions.TopRight:
            return 0 + yMargin
        case ItemPositions.BottomLeft:
        case ItemPositions.Bottom:
        case ItemPositions.BottomRight:
            return parent.height - height + yMargin
        case ItemPositions.Right:
        case ItemPositions.Left:
            return (parent.height - height) / 2 + yMargin
        }
        console.warn("Unknown Position:", position)
        return (parent.height - height) / 2
    }

    ListView {
        id: listView
        anchors.fill: parent

        orientation: ListView.Vertical
        interactive: height<contentHeight
        spacing: 0

        verticalLayoutDirection: {
            if (!parent)
                return 0
            switch (root.position) {
            case ItemPositions.TopLeft:
            case ItemPositions.Top:
            case ItemPositions.TopRight:
            case ItemPositions.Left:
            case ItemPositions.Right:
                return ListView.TopToBottom
            case ItemPositions.BottomLeft:
            case ItemPositions.Bottom:
            case ItemPositions.BottomRight:
                return ListView.BottomToTop
            }
            console.warn("Unknown Position:", root.position)
            return ListView.TopToBottom
        }

        model: SnackbarManager.children
        delegate: Snackbar {
            width: ListView.view.width
            required property SnackbarObject qtObject
            required property var settings
            required severity
            required title
            required caption
            required details
            required button
            required closable
            required timeout
            required progress

            Component.onCompleted: qtObject.visible = true

            elevation: root.elevation
            filled: root.filled
            outlined: root.outlined

            onClicked: {
                var settings = {
                    // "textFormat": TextEdit.RichText,
                    "title": title,
                    "message": caption,
                    "infos": details,
                    "diagnose": false
                }
                DialogManager.showText(settings)
            }

            onClosed: {
                qtObject.close()
                settings?.onClosed()
            }

            onAccepted: {
                qtObject.accept()
                settings?.onAccepted()
            }
        }

        add: Transition {
            enabled: root.transitionsEnabled
            NumberAnimation { property: "opacity"; from: 0; to: 1; duration: root.transitionsDuration }
            NumberAnimation { property: "scale"; from: 0.8; to: 1; duration: root.transitionsDuration }
        }

        move: Transition {
            enabled: root.transitionsEnabled
            NumberAnimation { properties: "x,y"; duration: root.transitionsDuration }

            // ensure opacity and scale values return to 1.0
            NumberAnimation { property: "opacity"; to: 1.0 }
            NumberAnimation { property: "scale"; to: 1.0 }
        }

        displaced: Transition {
            enabled: root.transitionsEnabled
            NumberAnimation { properties: "x,y"; duration: root.transitionsDuration }

            // ensure opacity and scale values return to 1.0
            NumberAnimation { property: "opacity"; to: 1.0 }
            NumberAnimation { property: "scale"; to: 1.0 }
        }
    }
}

