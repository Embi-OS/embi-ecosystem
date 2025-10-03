import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.CheckDelegate {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                    Math.max(implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding))

    display: AbstractButton.TextBesideIcon

    topInset: Style.menuItemTopInset
    bottomInset: Style.menuItemBottomInset
    leftInset: Style.menuItemLeftInset
    rightInset: Style.menuItemRightInset

    leftPadding: Style.menuItemIconPadding + leftInset
    rightPadding: Style.menuItemIconPadding + rightInset
    topPadding: 0
    bottomPadding: 0
    spacing: Style.menuItemSpacing

    property color foregroundColor: Style.colorWhite
    property color backgroundColor: "transparent"
    property double radius: Style.menuItemCornerRadius

    font: Style.menuItemFont

    icon.width: Style.menuItemIconWidth
    icon.height: Style.menuItemIconWidth
    icon.color: foregroundColor
    icon.source: ""

    opacity: enabled ? 1.0 : 0.5
    Behavior on opacity { NumberAnimation { duration: 150 } }

    indicator: BasicCheckIndicator {
        x: root.text ? (root.mirrored ? root.leftPadding : root.width - width - root.rightPadding) : root.leftPadding + (root.availableWidth - width) / 2
        y: root.topPadding + (root.availableHeight - height) / 2
        checkState: root.checkState
    }

    contentItem: IconLabel {
        leftPadding: !root.mirrored ? 0 : root.indicator.width + root.spacing
        rightPadding: root.mirrored ? 0 : root.indicator.width + root.spacing

        spacing: root.spacing
        mirrored: root.mirrored
        display: root.display
        alignment: Qt.AlignVCenter | Qt.AlignLeft
        elide: Text.ElideRight
        maximumLineCount: 1

        icon.source: root.icon.source
        icon.width: Style.roundToIconSize(root.icon.width)
        icon.height: Style.roundToIconSize(root.icon.height)
        icon.color: root.icon.color
        icon.cache: root.icon.cache

        text: root.text
        font: root.font
        color: root.foregroundColor
    }

    property double backgroundImplicitWidth: Style.menuItemMinWidth
    property double backgroundImplicitHeight: Style.menuItemMinHeight

    background: Rectangle {
        implicitWidth: root.backgroundImplicitWidth
        implicitHeight: root.backgroundImplicitHeight
        radius: root.radius
        color: root.backgroundColor

        BasicVeil {
            anchors.fill: parent
            radius: root.radius

            relativeBackgroundColor: root.backgroundColor
            focussed: root.down || root.visualFocus || root.hovered
            pressed: root.pressed
        }
    }
}
