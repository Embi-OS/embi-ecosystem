import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.MenuBarItem {
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

    leftPadding: Style.menuItemIconPadding
    rightPadding: Style.menuItemIconPadding
    topPadding: 0
    bottomPadding: 0

    spacing: Style.menuItemSpacing

    opacity: enabled ? 1.0 : 0.5
    Behavior on opacity { NumberAnimation { duration: 150 } }

    property color relativeBackgroundColor: root.backgroundColor.a>0 ? root.backgroundColor : Style.colorPrimaryDark
    property color foregroundColor: {
        if(highlighted)
            return Style.colorAccent
        return ColorUtils.isDarkColor(relativeBackgroundColor) ? Style.colorWhite : Style.colorBlack;
    }
    property color backgroundColor: "transparent"
    property bool labelFillWidth: false

    font: Style.menuItemFont

    icon.width: Style.menuItemIconWidth
    icon.height: Style.menuItemIconWidth
    icon.color: foregroundColor
    icon.source: ""

    contentItem: IconLabel {
        opacity: 1.0
        font: root.font
        spacing: root.spacing
        display: root.display
        mirrored: root.mirrored
        icon.source: root.icon.source
        icon.width: Style.roundToIconSize(root.icon.width)
        icon.height: Style.roundToIconSize(root.icon.height)
        icon.color: root.icon.color
        icon.cache: root.icon.cache
        text: root.text
        color: root.foregroundColor
        labelFillWidth: root.labelFillWidth
    }

    background: Rectangle {
        implicitWidth: Style.menuItemMinHeight
        implicitHeight: Style.menuItemMinHeight
        color: root.backgroundColor

        BasicVeil {
            z: 100
            anchors.fill: parent

            relativeBackgroundColor: root.relativeBackgroundColor
            focussed: root.down || root.visualFocus || root.hovered
            pressed: root.pressed
        }
    }
}
