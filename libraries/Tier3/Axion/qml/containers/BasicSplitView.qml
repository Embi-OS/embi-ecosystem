pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.SplitView {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    spacing: Style.splitViewSpacing

    property double separatorSize: Style.splitViewSeparatorSize
    property color separatorColor: Style.colorPrimary

    handle: Item {
        implicitWidth: root.orientation === Qt.Horizontal ? root.separatorSize+root.spacing*2 : root.width
        implicitHeight: root.orientation === Qt.Horizontal ? root.height : root.separatorSize+root.spacing*2

        Rectangle {
            anchors.centerIn: parent
            implicitWidth: root.orientation === Qt.Horizontal ? root.separatorSize : parent.width
            implicitHeight: root.orientation === Qt.Horizontal ? parent.height : root.separatorSize
            color: (parent.T.SplitHandle.hovered || parent.T.SplitHandle.pressed) ? Style.colorAccent : root.separatorColor
            radius: Math.min(width, height)/2
        }

        Rectangle {
            property int length: Style.splitViewHandleSize
            readonly property int thickness: Style.splitViewHandleThickness
            anchors.centerIn: parent
            implicitWidth: root.orientation === Qt.Horizontal ? thickness : length
            implicitHeight: root.orientation === Qt.Horizontal ? length : thickness
            color: (parent.T.SplitHandle.hovered || parent.T.SplitHandle.pressed) ? Style.colorAccent : root.separatorColor
            radius: thickness

            SvgColorImage {
                anchors.centerIn: parent
                size: Style.splitViewHandleIconSize
                icon: root.orientation === Qt.Horizontal ? MaterialIcons.dotsVertical : MaterialIcons.dotsHorizontal
                color: ColorUtils.isDarkColor(root.separatorColor) ? Style.colorWhite : Style.colorBlack
            }
        }
    }
}
