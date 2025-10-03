pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.Container {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding)

    property double inset: Style.containerInset
    topInset: inset
    bottomInset: inset
    leftInset: inset
    rightInset: inset

    padding: Math.max(radius, borderWidth)
    topPadding: padding + topInset
    bottomPadding: padding + bottomInset
    leftPadding: padding + leftInset
    rightPadding: padding + rightInset

    spacing: Style.containerSpacing

    opacity: enabled ? 1.0 : 0.5
    Behavior on opacity { NumberAnimation { duration: 150 } }

    property int edge: 0
    readonly property bool isOnTheRight: edge & Qt.RightEdge
    readonly property bool isOnTheLeft: edge & Qt.LeftEdge
    readonly property bool isOnTheTop: edge & Qt.TopEdge
    readonly property bool isOnTheBottom: edge & Qt.BottomEdge

    property bool drawFrame: false

    property double elevation: Style.flat ? 0 : 4
    property color color: Style.colorPrimary
    property color borderColor: Style.colorPrimary
    property double borderWidth: drawFrame ? Style.containerBorderWidth : 0
    property double radius: drawFrame ? Style.containerRadius : 0
    property bool backgroundVisible: true

    background: Item {
        visible: root.backgroundVisible

        layer.enabled: root.elevation>0
        layer.effect: BasicElevationEffect {
            elevation: root.elevation
        }

        Rectangle {
            visible: root.drawFrame
            anchors.fill: parent
            anchors.topMargin: root.isOnTheTop ? -(radius+border.width): 0
            anchors.bottomMargin: root.isOnTheBottom ? -(radius+border.width): 0
            anchors.leftMargin: root.isOnTheLeft ? -(radius+border.width): 0
            anchors.rightMargin: root.isOnTheRight ? -(radius+border.width): 0

            radius: root.radius
            color: root.color
            border.width: root.borderWidth
            border.color: root.borderColor
        }
    }
}
