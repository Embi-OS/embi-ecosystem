import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.TabButton {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    property double inset: Style.tabButtonInset
    topInset: inset
    bottomInset: inset
    leftInset: inset
    rightInset: inset

    padding: Style.tabButtonPadding + inset
    leftPadding: Style.tabButtonPadding + leftInset
    rightPadding: Style.tabButtonPadding + rightInset
    spacing: Style.tabButtonSpacing

    opacity: enabled ? 1.0 : 0.5
    Behavior on opacity { NumberAnimation { duration: 150 } }

    font: Style.textTheme.button

    property bool highlighted: false
    property color relativeBackgroundColor: root.backgroundColor.a>0 ? root.backgroundColor : Style.colorPrimary
    property color foregroundColor: {
        if(highlighted || checked)
            return Style.colorAccent
        return ColorUtils.isDarkColor(relativeBackgroundColor) ? Style.colorWhite : Style.colorBlack;
    }
    property color backgroundColor: "transparent" //checked ? Style.colorPrimaryDark : "transparent"
    property bool labelFillWidth: false

    readonly property double backgroundImplicitWidth: Style.tabButtonMinWidth
    readonly property double backgroundImplicitHeight: Style.tabButtonMinHeight
    property double radius: Style.tabButtonRadius

    icon.width: Style.tabButtonIconSize
    icon.height: Style.tabButtonIconSize
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

        color: root.backgroundColor
        radius: root.radius

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
