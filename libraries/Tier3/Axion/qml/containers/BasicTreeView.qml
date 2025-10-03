pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Utils
import Eco.Tier1.Models
import Eco.Tier3.Axion

TreeView {
    id: root

    clip: true
    flickableDirection: Flickable.VerticalFlick
    interactive: true
    reuseItems: false

    resizableColumns: false
    resizableRows: false

    columnWidthProvider: root.defaultColumnWidthProvider
    rowHeightProvider: root.defaultRowHeightProvider

    property double scrollBarSpacing: 5
    property double scrollIndicatorSpacing: 5

    readonly property bool verticalScrollBarVisible: (root.ScrollBar.vertical && root.ScrollBar.vertical.visible) || (root.ScrollIndicator.vertical && root.ScrollIndicator.vertical.visible)
    readonly property double verticalScrollBarWidth: (root.ScrollBar.vertical && root.ScrollBar.vertical.visible) ? (root.ScrollBar.vertical.width+root.scrollBarSpacing) : 0
    readonly property double verticalScrollIndicatorWidth: (root.ScrollIndicator.vertical && root.ScrollIndicator.vertical.visible) ? (root.ScrollIndicator.vertical.width+root.scrollIndicatorSpacing) : 0
    readonly property double verticalScrollerWidth: Math.max(verticalScrollBarWidth, verticalScrollIndicatorWidth)
    readonly property double viewWidth: root.width - verticalScrollerWidth

    readonly property bool horizontalScrollBarVisible: (root.ScrollBar.horizontal && root.ScrollBar.horizontal.visible) || (root.ScrollIndicator.horizontal && root.ScrollIndicator.horizontal.visible)
    readonly property double horizontalScrollBarHeight: (root.ScrollBar.horizontal && root.ScrollBar.horizontal.visible) ? (root.ScrollBar.horizontal.height+root.scrollBarSpacing) : 0
    readonly property double horizontalScrollIndicatorHeight: (root.ScrollIndicator.horizontal && root.ScrollIndicator.horizontal.visible) ? (root.ScrollIndicator.horizontal.height+root.scrollIndicatorSpacing) : 0
    readonly property double horizontalScrollerHeight: Math.max(horizontalScrollBarHeight, horizontalScrollIndicatorHeight)
    readonly property double viewHeight: root.height - horizontalScrollerHeight

    property bool isCompleted: false
    Component.onCompleted: isCompleted=true

    onViewWidthChanged: {
        forceLayoutThrottled()
    }

    SignalTrailingDebouncer {
        id: forceLayoutThrottler
        timeout: 0
        onTriggered: {
            root.forceLayout()
        }
    }

    function forceLayoutThrottled() {
        if(isCompleted)
            forceLayoutThrottler.throttle();
    }

    function defaultColumnWidthProvider(column: int): real {
        if(root.columns<=1)
            return root.viewWidth;
        var w = root.explicitColumnWidth(column)
        if (w >= 0)
            return w;
        return root.implicitColumnWidth(column);
    }

    function defaultRowHeightProvider(row: int): real {
        var h = root.explicitRowHeight(row)
        if (h >= 0)
            return h;
        if (!root.isRowLoaded(row))
            return -1;
        h = root.implicitRowHeight(row);
        return Math.max(1, h);
    }

    property Component itemDelegate: BasicTreeDelegate {
        property StandardObject qtObject
        text: qtObject?.text ?? ""
        onClicked: qtObject.toggle()
    }

    delegate: TreeViewDelegateLoader {
        required treeView
        required model
        required index
        required expanded
        required hasChildren
        required depth
        required qtObject

        objectName: qtObject?.text ?? ""
        delegate: qtObject?.delegate ?? root.itemDelegate
        unfolded: qtObject?.unfolded ?? false

        onVisibleChanged: {
            if(isReady && visible)
                root.forceLayoutThrottled()
        }
        onImplicitHeightChanged: {
            if(isReady && visible)
                root.forceLayoutThrottled()
        }
    }
}
