pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.TabBar {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding)

    property double inset: Style.tabBarInset
    topInset: inset
    bottomInset: inset
    leftInset: inset
    rightInset: inset

    padding: Style.tabBarPadding + inset
    topPadding: padding + topInset
    bottomPadding: padding + bottomInset
    leftPadding: padding + leftInset
    rightPadding: padding + rightInset

    opacity: enabled ? 1.0 : 0.5
    Behavior on opacity { NumberAnimation { duration: 150 } }

    property color backgroundColor: Style.colorPrimary
    spacing: Style.tabBarSpacing

    property double elevation: Style.tabBarElevation

    contentItem: ListView {
        model: root.contentModel
        currentIndex: root.currentIndex

        clip: true
        spacing: root.spacing
        orientation: ListView.Horizontal
        boundsBehavior: Flickable.StopAtBounds
        flickableDirection: Flickable.AutoFlickIfNeeded
        snapMode: ListView.SnapToItem

        highlightMoveDuration: 250
        highlightResizeDuration: 0
        highlightFollowsCurrentItem: true
        highlightRangeMode: ListView.ApplyRange
        preferredHighlightBegin: 48
        preferredHighlightEnd: width - 48

        highlight: Item {
            Rectangle {
                anchors.centerIn: parent
                height: parent.height-Style.tabButtonInset*2
                width: parent.width-Style.tabButtonInset*2
                radius: Style.tabButtonRadius
                color: Style.colorPrimaryDark
            }
        }
    }

    background: Rectangle {
        color: root.backgroundColor

        radius: Style.tabButtonRadius+Style.tabButtonInset

        layer.enabled: root.elevation>0
        layer.effect: BasicElevationEffect {
            elevation: root.elevation
        }
    }
}
