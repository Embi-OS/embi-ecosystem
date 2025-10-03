pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.AbstractButton {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    property string hint: ""
    property bool flat: false
    property bool highlighted: false

    font: Style.textTheme.button

    topInset: Style.buttonInset
    bottomInset: Style.buttonInset
    leftInset: Style.buttonInset
    rightInset: Style.buttonInset

    padding: Style.buttonIconPadding+Style.buttonInset
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
        return down ? Style.fabButtonPressedElevation : Style.fabButtonElevation
    }

    property double backgroundImplicitSize: Style.buttonFabSize
    property double radius: background.height/2

    icon.width: root.height-2*root.padding
    icon.height: root.height-2*root.padding
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
    }

    background: Rectangle {
        implicitWidth: root.backgroundImplicitSize
        implicitHeight: root.backgroundImplicitSize

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

        BasicToolTip {
            text: root.hint
            delay: 50
            visible: root.hovered && root.hint
            ExtraPosition.position: ItemPositions.Top
            ExtraPosition.margins: 5
        }
    }
}
