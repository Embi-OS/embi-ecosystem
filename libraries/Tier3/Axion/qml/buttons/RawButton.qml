pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.AbstractButton {
    id: root

    readonly property double buttonImplicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    readonly property double buttonImplicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    property double preferredWidth: 0
    property double preferredHeight: 0

    implicitWidth: Math.max(buttonImplicitWidth, preferredWidth)
    implicitHeight: Math.max(buttonImplicitHeight, preferredHeight)

    property bool round: Style.buttonRound
    property bool flat: false
    property bool highlighted: false

    font: Style.textTheme.button

    property double inset: Style.buttonInset
    topInset: inset
    bottomInset: inset
    leftInset: inset
    rightInset: inset

    readonly property bool hasIcon: icon.name.length > 0 || icon.source!==Qt.url("")

    padding: Style.buttonPadding
    leftPadding: {
        if(hasIcon && !mirrored || text === "")
            return iconPadding + leftInset
        return padding + leftInset
    }
    rightPadding: {
        if(hasIcon && mirrored || text === "")
            return iconPadding + rightInset
        return padding + rightInset
    }
    topPadding: topInset
    bottomPadding: bottomInset
    spacing: Style.buttonSpacing

    opacity: enabled ? 1.0 : 0.5
    Behavior on opacity { NumberAnimation { duration: 150 } }

    property color colorHighlight: Style.colorAccent
    property color color: Style.colorPrimary
    property color relativeBackgroundColor: root.backgroundColor.a>0 ? root.backgroundColor : Style.colorBackground
    property color foregroundColor: {
        if(flat && highlighted)
            return root.colorHighlight
        return ColorUtils.isDarkColor(root.relativeBackgroundColor) ? Style.colorWhite : Style.colorBlack
    }
    property color backgroundColor: {
        if(flat)
            return "transparent"
        return highlighted ? root.colorHighlight : root.color
    }

    property double elevation: {
        if(flat)
            return 0
        return down ? Style.buttonPressedElevation : Style.buttonElevation
    }

    property int radius: root.round ? backgroundImplicitHeight/2 : Style.buttonRadius
    property bool labelFillWidth: false

    readonly property double backgroundImplicitWidth: Style.buttonMinWidth
    readonly property double backgroundImplicitHeight: Style.buttonMinHeight

    property int iconPadding: Style.buttonIconPadding
    icon.width: root.height-2*root.iconPadding
    icon.height: root.height-2*root.iconPadding
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

        layer.enabled: root.enabled && (root.backgroundColor.a > 0) && root.elevation>0
        layer.effect: BasicElevationEffect {
            elevation: root.elevation
        }

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
