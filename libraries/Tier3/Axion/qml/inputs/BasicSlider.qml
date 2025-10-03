import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.Slider {
    id: root

    property int size: Style.sliderSize

    property color color: Style.colorAccent
    property color backgroundColor: Style.colorPrimaryDarker
    property bool readOnly: false
    property alias hint: toolTip.text

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitHandleWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitHandleHeight + topPadding + bottomPadding)

    padding: 6

    handle: Rectangle {
        x: root.leftPadding + root.visualPosition * (root.availableWidth - width)
        y: root.topPadding + root.availableHeight / 2 - height / 2
        implicitWidth: root.size*2
        implicitHeight: root.size*2
        radius: root.size
        color: root.color
        visible: !root.readOnly

        BasicVeil {
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            width: 32; height: 32
            radius: 16
            pressed: root.pressed
            focussed: root.pressed || root.visualFocus || root.hovered
            color: ColorUtils.transparent(root.color, 0.4)
        }

        BasicToolTip {
            id: toolTip
            ExtraPosition.position: ItemPositions.Top
            ExtraPosition.margins: 5
            text: FormatUtils.realToString(root.value,1)
            delay: 50
            visible: root.hovered || root.pressed
        }
    }

    background: Item {
        x: root.leftPadding + (root.horizontal ? 0 : (root.availableWidth - width) / 2)
        y: root.topPadding + (root.horizontal ? (root.availableHeight - height) / 2 : 0)
        implicitWidth: root.horizontal ? 200 : 48
        implicitHeight: root.horizontal ? 48 : 200
        width: root.horizontal ? root.availableWidth : root.size
        height: root.horizontal ? root.size : root.availableHeight

        Rectangle {
            x: (root.horizontal ? (root.implicitHandleWidth / 2) : 0)
            y: (root.horizontal ? 0 : (root.implicitHandleHeight / 2))
            width: parent.width - (root.horizontal ? (root.implicitHandleWidth) : 0)
            height: parent.height - (root.horizontal ? 0 : (root.implicitHandleHeight))
            scale: root.horizontal && root.mirrored ? -1 : 1
            radius: Math.min(width, height) / 2
            color: root.backgroundColor

            Rectangle {
                x: root.horizontal ? 0 : (parent.width - width) / 2
                y: root.horizontal ? (parent.height - height) / 2 : root.visualPosition * parent.height
                width: root.horizontal ? root.position * parent.width : root.size
                height: root.horizontal ? root.size : root.position * parent.height
                radius: Math.min(width, height) / 2
                color: root.color
            }
        }
    }
}
