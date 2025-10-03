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

    property double inset: Style.railBarInset
    topInset: inset
    bottomInset: inset
    leftInset: inset
    rightInset: inset

    padding: Style.railBarPadding + inset
    topPadding: padding + topInset
    bottomPadding: padding + bottomInset
    leftPadding: padding + leftInset
    rightPadding: padding + rightInset

    opacity: enabled ? 1.0 : 0.5
    Behavior on opacity { NumberAnimation { duration: 150 } }

    property color backgroundColor: Style.colorPrimary
    spacing: Style.railBarSpacing

    property double elevation: Style.railBarElevation

    ButtonGroup {
        buttons: root.contentChildren
        onCheckedButtonChanged: root.setCurrentIndex(Math.max(0, buttons.indexOf(checkedButton)))
    }

    contentWidth: {
        var maxWidth=0
        for(const child of contentChildren) {
            maxWidth = Math.max(maxWidth, child.implicitWidth)
        }
        return maxWidth;
    }

    contentHeight: {
        var totalHeight=Math.max(0, count-1)*root.spacing
        for(const child of contentChildren) {
            totalHeight += child.implicitHeight
        }
        return totalHeight;
    }

    onCurrentIndexChanged: {
        var button = root.contentModel.get(currentIndex) as T.TabButton
        if(button && !button.checked)
            button.checked = true
    }

    contentItem: ListView {
        model: root.contentModel
        currentIndex: root.currentIndex

        clip: true
        spacing: root.spacing
        orientation: ListView.Vertical
        boundsBehavior: Flickable.StopAtBounds
        flickableDirection: Flickable.AutoFlickIfNeeded
        snapMode: ListView.SnapToItem

        highlightMoveDuration: 250
        highlightResizeDuration: 0
        highlightFollowsCurrentItem: true

        highlight: Item {
            z: -1
            Rectangle {
                anchors.centerIn: parent
                height: parent.height-Style.sideButtonInset*2
                width: parent.width-Style.sideButtonInset*2
                radius: Style.sideButtonRadius
                color: Style.colorPrimaryDark
                border.color: Style.colorAccent
                border.width: Style.sideButtonBorderWidth
            }
        }
    }

    background: Rectangle {
        color: root.backgroundColor

        layer.enabled: root.elevation>0
        layer.effect: BasicElevationEffect {
            elevation: root.elevation
        }
    }
}
