import QtQuick
import Eco.Tier3.Axion

BasicStackView {
    id: root

    // StackView is badly codded by Qt so popCacheItem are necessary for StackView to work properly
    readonly property Item popCacheItem: Item {}

    required property BasicNavObject initialObject

    initialItem: initialObject.navItem

    function clearObjects() {
        root.popToIndex(0);
        initialObject.index=0;
        root.replaceCurrentItem(initialItem);
    }

    function popObject(obj: BasicNavObject) {
        Log.startElapsed()
        if(root.popCacheItem)
            root.pushItem(root.popCacheItem)
        root.popToItem(obj.item)
        Log.trace(("popObject took: %1 ms").arg(Log.elapsed()))
    }

    function pushObject(obj: BasicNavObject) {
        Log.startElapsed()
        obj.index = root.depth;
        if(obj.properties)
            obj.item = root.pushItem(obj.navItem, obj.properties) as BasicNavItem
        else
            obj.item = root.pushItem(obj.navItem) as BasicNavItem
        obj.properties = null
        Log.trace(("pushObject took: %1 ms").arg(Log.elapsed()))
    }

    function replaceCurrentObject(obj: BasicNavObject) {
        Log.startElapsed()
        obj.index = root.depth-1;
        if(obj.properties)
            obj.item = root.replaceCurrentItem(obj.navItem, obj.properties) as BasicNavItem
        else
            obj.item = root.replaceCurrentItem(obj.navItem) as BasicNavItem
        obj.properties = null
        Log.trace(("replaceCurrentObject took: %1 ms").arg(Log.elapsed()))
    }
}
