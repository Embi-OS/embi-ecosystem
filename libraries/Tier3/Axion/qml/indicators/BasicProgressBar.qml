import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.ProgressBar {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding) + labelWidthOffset
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding) + labelHeightOffset


    readonly property double labelWidthOffset: 0
    readonly property double labelHeightOffset: 0

    property color backgroundColor: Style.colorPrimaryDark
    property color progressColor: Style.colorAccent

    property int indeterminateDuration: 1000
    property double indeterminanteRatio: 0.2
    property double lineWidth: Style.busyIndicatorLineWidth
    property int capStyle: Qt.RoundCap
    readonly property double radius: capStyle===Qt.RoundCap ? lineWidth/2 : 0

    property bool labelVisible: true
    property int labelPosition: ItemPositions.TopEnd
    property double labelMargins: 10
    readonly property string defaultLabel: FormatUtils.realToString(root.position*100.0,2)+"%"
    property string text: defaultLabel
    font: Style.textTheme.subtitle1

    BasicLabel {
        id: label
        visible: root.labelVisible && !root.indeterminate
        font: root.font
        text: root.text

        ExtraPosition.targetParent: root.contentItem
        ExtraPosition.position: root.labelPosition
        ExtraPosition.margins: root.labelMargins
    }

    contentItem: Item {
        implicitWidth: 300
        implicitHeight: root.lineWidth
        scale: root.mirrored ? -1 : 1

        Rectangle {
            height: parent.height
            width: root.indeterminate ? height : root.position * parent.width
            radius: root.radius
            color: root.progressColor

            SequentialAnimation on width {
                loops: Animation.Infinite
                running: root.indeterminate && root.visible
                NumberAnimation { from: root.contentItem.height; to: root.contentItem.width*root.indeterminanteRatio; duration: root.indeterminateDuration*0.2 }
                PauseAnimation { duration: root.indeterminateDuration*0.6 }
                NumberAnimation { to: root.contentItem.height; duration: root.indeterminateDuration*0.2 }
                NumberAnimation { to: root.contentItem.height; duration: 0 }
            }

            SequentialAnimation on x {
                loops: Animation.Infinite
                running: root.indeterminate && root.visible
                PauseAnimation { duration: root.indeterminateDuration*0.2 }
                NumberAnimation { from: 0; to: root.contentItem.width-root.contentItem.width*root.indeterminanteRatio; duration: root.indeterminateDuration*0.6 }
                NumberAnimation { to: root.contentItem.width-root.contentItem.height; duration: root.indeterminateDuration*0.2 }
                NumberAnimation { to: 0; duration: 0 }
            }
        }
    }

    background: Rectangle {
        implicitWidth: 300
        implicitHeight: root.lineWidth

        color: root.backgroundColor
        radius: root.radius
    }
}
