import QtQuick
import QtQuick.Templates as T
import Eco.Tier1.Utils
import Eco.Tier3.Axion

T.Control {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    spacing: 5
    horizontalPadding: spacing

    property int interval: 1000
    property bool running: true

    property bool showAverage: true
    property bool showMinimum: true
    property bool showMaximum: true
    property bool showJitter: true

    FrameTimer {
        id: frameTimer
        window: root.Window.window
        running: root.visible && root.running
        interval: root.interval
    }

    contentItem: RowLayout {
        spacing: root.spacing

        BasicBusyIndicator {
            Layout.alignment: Qt.AlignVCenter
            backgroundColor: "transparent"
            size: 14
            lineWidth: 0
        }

        BasicLabel {
            id: label
            Layout.alignment: Qt.AlignVCenter
            font: Style.textTheme.body2
            Binding on text {
                when: root.visible && root.running
                delayed: true
                value: (root.showAverage ? ("AVG: " + FormatUtils.realToString(frameTimer.averageFps, 1) + (root.showMinimum||root.showMaximum||root.showJitter?" / ":"")) : "") +
                       (root.showMinimum ? ("MIN: " + FormatUtils.realToString(frameTimer.minimumFps, 1) + (root.showMaximum||root.showJitter?" / ":"")) : "") +
                       (root.showMaximum ? ("MAX: " + FormatUtils.realToString(frameTimer.maximumFps, 1) + (root.showJitter?" / ":"")) : "") +
                       (root.showJitter ? ("JIT: " + FormatUtils.realToString(frameTimer.jitterFps, 1)) : "")
            }
        }
    }
}
