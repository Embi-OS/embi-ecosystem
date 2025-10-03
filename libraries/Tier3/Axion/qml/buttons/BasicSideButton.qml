import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.TabButton {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    property double inset: Style.sideButtonInset
    topInset: inset
    bottomInset: inset
    leftInset: inset
    rightInset: inset

    padding: Style.sideButtonPadding + inset
    spacing: Style.sideButtonSpacing

    opacity: enabled ? 1.0 : 0.5
    Behavior on opacity { NumberAnimation { duration: 150 } }

    font: Style.textTheme.button

    property bool highlighted: false
    property color relativeBackgroundColor: root.backgroundColor.a>0 ? root.backgroundColor : Style.colorPrimaryDark
    property color foregroundColor: {
        if(highlighted || checked)
            return Style.colorAccent
        return ColorUtils.isDarkColor(relativeBackgroundColor) ? Style.colorWhite : Style.colorBlack;
    }
    property color backgroundColor: Style.colorTransparent
    property color borderColor: Style.colorTransparent
    property double borderWidth: Style.sideButtonBorderWidth

    readonly property double backgroundImplicitWidth: Style.sideButtonMinWidth
    readonly property double backgroundImplicitHeight: Style.sideButtonMinHeight
    property double radius: Style.sideButtonRadius

    icon.width: Style.sideButtonIconSize
    icon.height: Style.sideButtonIconSize
    icon.color: foregroundColor
    icon.source: ""

    display: IconLabelDisplays.TextUnderIcon

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
    }

    background: Rectangle {
        implicitWidth: root.backgroundImplicitWidth
        implicitHeight: root.backgroundImplicitHeight

        color: root.backgroundColor
        radius: root.radius
        border.color: root.borderColor
        border.width: root.borderWidth

        BasicVeil {
            z: 100
            anchors.fill: parent
            radius: root.radius

            relativeBackgroundColor: root.relativeBackgroundColor
            focussed: root.down || root.visualFocus || root.hovered
            pressed: root.pressed
        }
    }
}
