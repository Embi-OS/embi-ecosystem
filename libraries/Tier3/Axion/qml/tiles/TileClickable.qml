pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.AbstractButton {
    id: root

    implicitWidth: implicitBackgroundWidth + leftInset + rightInset
    implicitHeight: implicitBackgroundHeight + topInset + bottomInset

    property double inset: Style.tileInset
    topInset: inset
    bottomInset: inset
    leftInset: inset
    rightInset: inset

    spacing: Style.tileSpacing
    padding: Style.tilePadding + inset
    font: Style.textTheme.body1

    opacity: enabled ? 1.0 : 0.5

    property bool flat: false
    property bool highlighted: false

    property int radius: Style.tileRadius
    readonly property double backgroundImplicitWidth: Style.tileMinWidth
    readonly property double backgroundImplicitHeight: Style.tileMinHeight

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
        return down ? 0 : Style.tileElevation
    }

    property string value

    icon.width: root.height-2*root.padding
    icon.height: root.height-2*root.padding
    icon.color: foregroundColor
    icon.source: ""

    contentItem: ColumnLayout {
        spacing: root.spacing
        BasicLabel {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            opacity: 1.0
            font: root.font
            text: root.text
            color: root.foregroundColor
            wrapMode: Text.Wrap
        }

        SvgColorImage {
            Layout.fillWidth: true
            Layout.fillHeight: true
            iconWidth: width //Style.roundToIconSize(width)
            iconHeight: height //Style.roundToIconSize(height)
            aspectRatio: Qt.KeepAspectRatio
            icon: root.icon.source
            color: root.icon.color
        }

        BasicLabel {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            opacity: 1.0
            font: root.font
            text: root.value
            color: root.foregroundColor
        }
    }

    background: Rectangle {
        implicitWidth: root.backgroundImplicitWidth
        implicitHeight: root.backgroundImplicitHeight

        radius: root.radius
        color: root.backgroundColor

        layer.enabled: root.enabled && (root.backgroundColor.a > 0) && root.elevation
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
