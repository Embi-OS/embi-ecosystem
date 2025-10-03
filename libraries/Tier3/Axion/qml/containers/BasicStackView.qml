import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.StackView {
    id: root

    // StackView is badly codded by Qt so transition are necessary for StackView to work properly
    popEnter: Transition {}
    popExit: Transition {}
    pushEnter: Transition {}
    pushExit: Transition {}
    replaceEnter: Transition {}
    replaceExit: Transition {}

    function popLast() {
        Log.startElapsed()
        root.popCurrentItem();
        Log.trace(("popLast took: %1 ms").arg(Log.elapsed()))
    }

    function pushComponent(comp: Component, properties: var): Item {
        Log.startElapsed()
        var item = root.pushItem(comp, properties)
        Log.trace(("pushComponent took: %1 ms").arg(Log.elapsed()))
        return item;
    }
}
