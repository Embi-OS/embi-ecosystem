import QtQuick
import Eco.Tier1.Utils
import Eco.Tier3.Axion

Item {
    id: root

    z: Infinity
    visible: root.enabled

    property int size: 60

    property color color: Style.colorVariant

    property alias window: mouseEventNotifier.window

    MouseEventNotifier {
        id: mouseEventNotifier
        enabled: root.enabled
        onMousePressed: clickAnimation.restart()
    }

    Rectangle {
        id: clickIndicator
        visible: JS.pointIsValid(mouseEventNotifier.scenePosition) && (mouseEventNotifier.pressed || clickAnimation.running)
        color: ColorUtils.transparent(root.color, 0.6)
        width: root.size
        height: width
        radius: Math.min(width, height)/2

        x: mouseEventNotifier.scenePosition.x - root.size/2
        y: mouseEventNotifier.scenePosition.y - root.size/2

        ScaleAnimator {
            id: clickAnimation
            target: clickIndicator
            from: 0.0
            to: 1.0
            duration: 500
            easing.type: Easing.OutElastic
        }
    }
}
