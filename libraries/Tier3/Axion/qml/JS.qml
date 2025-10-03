pragma Singleton
import QtQuick

QtObject {
    id: root

//──────────────────────────────────────────────────────────────────────
// Math
//──────────────────────────────────────────────────────────────────────

    function pointIsValid(point: point): bool {
        return !(point.x === 0 && point.y === 0);
    }

//──────────────────────────────────────────────────────────────────────
// singleshotTimer
//──────────────────────────────────────────────────────────────────────

    function singleshotTimer(interval: int, callback: var) {
        var settings = {
            "interval": interval,
            "repeat": false
        }

        var delayComponent = Qt.createComponent("QtQml", "Timer");
        var delayTimer = delayComponent.createObject(root, settings) as Timer;

        delayTimer.triggered.connect(function () {
            callback();
            delayTimer.destroy();
        });
        delayTimer.start();
    }
}
