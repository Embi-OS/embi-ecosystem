import QtQuick
import Eco.Tier1.Utils
import Eco.Tier3.Axion

ListView {
    id: root

    clip: true
    orientation: ListView.Vertical
    interactive: true
    reuseItems: true

    property double scrollBarSpacing: 5
    property double scrollIndicatorSpacing: 5

    readonly property double verticalScrollBarWidth: (root.ScrollBar.vertical && root.ScrollBar.vertical.visible) ? (root.ScrollBar.vertical.width+root.scrollBarSpacing) : 0
    readonly property double verticalScrollIndicatorWidth: (root.ScrollIndicator.vertical && root.ScrollIndicator.vertical.visible) ? (root.ScrollIndicator.vertical.width+root.scrollIndicatorSpacing) : 0
    readonly property double verticalScrollerWidth: Math.max(verticalScrollBarWidth, verticalScrollIndicatorWidth)
    readonly property double viewWidth: root.width - verticalScrollerWidth

    readonly property double horizontalScrollBarHeight: (root.ScrollBar.horizontal && root.ScrollBar.horizontal.visible) ? (root.ScrollBar.horizontal.height+root.scrollBarSpacing) : 0
    readonly property double horizontalScrollIndicatorHeight: (root.ScrollIndicator.horizontal && root.ScrollIndicator.horizontal.visible) ? (root.ScrollIndicator.horizontal.height+root.scrollIndicatorSpacing) : 0
    readonly property double horizontalScrollerHeight: Math.max(horizontalScrollBarHeight, horizontalScrollIndicatorHeight)
    readonly property double viewHeight: root.height - horizontalScrollerHeight

    SignalTrailingDebouncer {
        id: gotoIndexThrottler
        timeout: 16
        onTriggered: root.gotoIndexNow(index, mode)
        property int index: -1
        property int mode: ListView.Center
    }

    function gotoIndex(index: int, mode: int) {
        if(index<0 || index>=root.count)
            return;

        gotoIndexThrottler.index=index;
        gotoIndexThrottler.mode=mode;
        gotoIndexThrottler.throttle();
    }

    function gotoIndexNow(index: int, mode: int) {
        if(index<0 || index>=root.count)
            return;

        var posProperty = "";
        if(root.orientation===ListView.Vertical)
            posProperty = "contentY";
        else
            posProperty = "contentX";

        anim.running = false;
        anim.property = posProperty;
        var pos = root[posProperty];
        root.positionViewAtIndex(index, mode);
        anim.from = pos;
        anim.to = root[posProperty];
        anim.running = true;
    }

    SignalTrailingDebouncer {
        id: gotoSubItemThrottler
        timeout: 16
        onTriggered: root.gotoSubItemNow(item, mode)
        property Item item: null
        property int mode: ListView.Center
    }

    function gotoSubItem(item: Item, mode: int) {
        if(!item)
            return;

        gotoSubItemThrottler.item=item;
        gotoSubItemThrottler.mode=mode;
        gotoSubItemThrottler.throttle();
    }

    function gotoSubItemNow(item: Item, mode: int) {
        if(!item)
            return;

        var relativePos = item.mapToItem(root.contentItem,0,0);
        var rootIndex = root.indexAt(relativePos.x, relativePos.y);
        var rootItem = root.itemAtIndex(rootIndex);
        if(rootItem===item) {
            root.gotoIndexNow(rootIndex, mode)
        }
        else {
            var posProperty = "";
            var pos = 0;
            var itemPos = 0;
            var itemSize = 0;
            var viewSize = 0;
            var viewContentSize = 0;

            if(root.orientation===ListView.Vertical) {
                posProperty = "contentY";
                pos = root.contentY;
                itemPos = relativePos.y;
                itemSize = item.height;
                viewSize = root.height;
                viewContentSize = root.contentItem.height;
            }
            else {
                posProperty = "contentX";
                pos = root.contentX;
                itemPos = relativePos.x;
                itemSize = item.width;
                viewSize = root.width;
                viewContentSize = root.contentItem.width;
            }

            var itemEndPos = (itemPos+itemSize);

            switch (mode) {
            case ListView.Beginning:
                pos = itemPos;
                break;
            case ListView.Center:
                pos = itemPos-(viewSize-itemSize)/2;
                break;
            case ListView.End:
                pos = itemPos-viewSize+itemSize;
                break;
            case ListView.Visible:
                if (itemPos >= pos + viewSize)
                    pos = itemEndPos - viewSize;
                else if (itemEndPos<= pos)
                    pos = itemPos;
                break;
            case ListView.Contain:
                if(itemEndPos>(pos+viewSize))
                    pos = itemPos+(itemSize-viewSize);
                if(itemPos<pos)
                    pos = itemPos;
                break;
            case ListView.SnapPosition:
                pos = preferredHighlightBegin;
                break;
            default:
                return;
            }

            anim.running = false;
            anim.property = posProperty;
            anim.from = root[posProperty];
            anim.to = MathUtils.bound(0, pos, Math.max(viewContentSize-viewSize,0))
            anim.running = true;
        }
    }

    NumberAnimation {
        id: anim
        target: root
        duration: 250
        easing.type: Easing.InOutQuad
        onFinished: running=false
    }
}
