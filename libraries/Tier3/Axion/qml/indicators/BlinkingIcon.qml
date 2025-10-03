import QtQuick
import Eco.Tier1.Utils
import Eco.Tier3.Axion

SvgColorImage {
    id: root

    property bool blink: true
    property int interval: 350
    property int fadeDuration: 30

    onBlinkChanged: throttler.throttle()
    onIntervalChanged: throttler.throttle()

    SignalTrailingThrottler {
        id: throttler
        timeout: 16
        onTriggered: root.evaluate()
    }

    function evaluate() {
        animation.stop();
        root.opacity=1.0;
        if(blink)
            animation.start();
    }

    SequentialAnimation on opacity {
        id: animation
        loops: Animation.Infinite
        NumberAnimation { from: 0; to: 1.0; duration: root.fadeDuration }
        PauseAnimation { duration: root.interval }
        NumberAnimation { from: 1.0; to: 0; duration: root.fadeDuration }
        PauseAnimation { duration: root.interval }
   }
}
