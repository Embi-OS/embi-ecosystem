pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion

Item {
    id: root

    anchors.fill: flickable

    property Flickable flickable: {
        let candidate = parent
        while (candidate) {
            var target = (candidate as Flickable)
            if (target) {
                return target
            }
            candidate = candidate.parent
        }
        return null;
    }

    readonly property double overshoot: flickable.verticalOvershoot
    readonly property double topOvershoot: Math.abs(Math.min(0,overshoot))
    readonly property double bottomOvershoot: Math.abs(Math.max(0,overshoot))

    readonly property double progress: Math.min(topOvershoot / threshold, 1)

    property double elevation: 4

    property double threshold: 150
    property int delay: 10
    property bool triggeredQueued: false

    signal triggered()

    Binding {
        // root.flickable.boundsMovement: Flickable.StopAtBounds
        root.flickable.boundsBehavior: Flickable.DragOverBounds
    }

    onProgressChanged: {
        if(root.triggeredQueued && root.progress <= 0) {
            root.triggeredQueued = false
            root.triggered()
        } else if (root.progress >= 1) {
            refreshTriggerTimer.running = true;
        } else {
            refreshTriggerTimer.running = false;
        }
    }

    Timer {
        id: refreshTriggerTimer
        interval: root.delay
        onTriggered: {
            if (root.progress >= 1) {
               root.triggeredQueued = true
            }
        }
    }

    Rectangle {
        parent: root.flickable
        anchors.horizontalCenter: root.horizontalCenter
        anchors.bottom: root.top
        // anchors.bottomMargin: -(height+root.topOvershoot)/2
        anchors.bottomMargin: -(root.topOvershoot)/1.6

        opacity: root.triggeredQueued ? 1 : Math.min(1,root.progress)
        rotation: root.triggeredQueued ? 0 : Math.min(1,root.progress)*360.0

        color: Style.colorPrimaryLight
        width: Style.buttonMinWidth
        height: width
        radius: height/2
        layer.enabled: root.elevation>0
        layer.effect: BasicElevationEffect {
            elevation: root.elevation
        }

        SvgIconLoader {
            anchors.centerIn: parent
            size: 32
            icon: root.triggeredQueued ? MaterialIcons.check : MaterialIcons.refresh
            color: root.triggeredQueued ? Style.colorSuccess : Style.colorWhite
        }
    }
}
