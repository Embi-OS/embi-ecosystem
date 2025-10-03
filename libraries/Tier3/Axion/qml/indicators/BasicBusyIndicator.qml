import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.BusyIndicator {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 0

    property color backgroundColor: Style.colorPrimaryDark
    property color progressColor: Style.colorAccent

    property double size: 60
    property int capStyle: Qt.RoundCap
    property double lineWidth: root.size<60 ? 0 : Style.busyIndicatorLineWidth

    running: visible

    contentItem: BusyIndicatorImpl {
        implicitWidth: root.size
        implicitHeight: root.size
        backgroundColor: root.backgroundColor
        color: root.progressColor
        capStyle: root.capStyle
        lineWidth: root.lineWidth

        running: root.running
        opacity: root.running ? 1 : 0
        Behavior on opacity { OpacityAnimator { duration: 250 } }
    }
}
