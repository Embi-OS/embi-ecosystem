import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.ToolButton {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    topInset: Style.toolButtonInset
    bottomInset: Style.toolButtonInset
    leftInset: Style.toolButtonInset
    rightInset: Style.toolButtonInset

    padding: Style.toolButtonPadding + Style.toolButtonInset
    spacing: Style.toolButtonSpacing

    opacity: enabled ? 1.0 : 0.5
    Behavior on opacity { NumberAnimation { duration: 150 } }

    font: Style.textTheme.button

    checkable: true
    highlighted: false
    property color relativeBackgroundColor: root.backgroundColor.a>0 ? root.backgroundColor : Style.colorPrimary
    property color foregroundColor: {
        if(highlighted)
            return Style.colorAccent
        return ColorUtils.isDarkColor(relativeBackgroundColor) ? Style.colorWhite : Style.colorBlack;
    }
    property color backgroundColor: highlighted ? Style.colorAccentFade : "transparent"
    property bool labelFillWidth: false

    readonly property double backgroundImplicitWidth: Style.toolButtonMinWidth
    readonly property double backgroundImplicitHeight: Style.toolButtonMinHeight
    property int radius: Style.toolButtonRadius

    icon.width: Style.toolButtonIconWidth
    icon.height: Style.toolButtonIconWidth
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
        implicitWidth: root.backgroundImplicitWidth
        implicitHeight: root.backgroundImplicitHeight

        radius: root.radius
        color: root.backgroundColor

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
