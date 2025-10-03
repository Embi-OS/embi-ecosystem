pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.Drawer {
    id: root

    parent: T.Overlay.overlay

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding)

    // topPadding: edge !== Qt.TopEdge ? Material.roundedScale : 0
    // bottomPadding: edge !== Qt.BottomEdge ? Material.roundedScale : 0

    enter: Transition {
        SmoothedAnimation { velocity: 5 }
        // NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; easing.type: Easing.OutCubic; duration: 150 }
    }

    exit: Transition {
        SmoothedAnimation { velocity: 5 }
        // NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; easing.type: Easing.OutCubic; duration: 150 }
    }

    T.Overlay.modal: BasicOverlay {}
    T.Overlay.modeless: BasicOverlay {}

    property double elevation: Style.drawerElevation

    property color backgroundColor: Style.colorPrimaryDark
    property color borderColor: Style.colorPrimary

    background: Item {
        layer.enabled: root.elevation>0
        layer.effect: BasicElevationEffect {
            elevation: root.elevation
        }

        Rectangle {
            anchors.fill: parent
            anchors.topMargin: root.edge === Qt.TopEdge ? -(radius+border.width): 0
            anchors.bottomMargin: root.edge === Qt.BottomEdge ? -(radius+border.width): 0
            anchors.leftMargin: root.edge === Qt.LeftEdge ? -(radius+border.width): 0
            anchors.rightMargin: root.edge === Qt.RightEdge ? -(radius+border.width): 0

            radius: Style.drawerRadius
            color: ColorUtils.transparent(root.backgroundColor, 1.0)
            border.width: Style.drawerBorderWidth
            border.color: ColorUtils.transparent(root.borderColor, 1.0)
        }
    }
}
