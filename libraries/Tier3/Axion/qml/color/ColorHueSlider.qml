import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.Slider {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitHandleWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitHandleHeight + topPadding + bottomPadding)

    from: 0.0
    to: 1.0
    padding: 6

    property color color: Style.colorAccent
    property int radius: 4

    handle: ColorPickerHandle {
        x: root.leftPadding + root.visualPosition * root.availableWidth - width / 2
        y: root.topPadding + root.availableHeight / 2 - height / 2
    }

    background: Item {
        x: root.leftPadding
        y: root.topPadding + (root.availableHeight - height) / 2
        implicitWidth: 280
        implicitHeight: 32
        width: root.availableWidth
        height: 32

        Rectangle {
            anchors.fill: parent
            border.width: Style.containerBorderWidth
            border.color: Style.colorPrimaryLight
            radius: root.radius
            gradient: ColorHueGradient {
                orientation: Gradient.Horizontal
            }
        }
    }
}
