import QtQuick
import Eco.Tier1.Utils
import Eco.Tier3.Axion

SvgColorImage {
    id: root

    property bool rotate: true
    property bool ccw: false
    property int duration: 4000

    onRotateChanged: throttler.throttle()
    onCcwChanged: throttler.throttle()

    SignalTrailingThrottler {
        id: throttler
        timeout: 16
        onTriggered: root.evaluate()
    }

    function evaluate() {
        animation.stop();
        root.rotation=0;
        if(rotate)
            animation.start();
    }

    RotationAnimation on rotation {
        id: animation
        from: 0
        to: root.ccw ? -360 : 360
        duration: root.duration
        direction : root.ccw ? RotationAnimation.Counterclockwise : RotationAnimation.Clockwise
        loops: Animation.Infinite
    }
}
