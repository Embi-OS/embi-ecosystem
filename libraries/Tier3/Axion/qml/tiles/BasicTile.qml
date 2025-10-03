pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.Control {
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

    property bool flat: false
    property bool highlighted: false
    property bool concealed: false

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

    property double elevation: flat ? 0 : Style.tileElevation

    property string text

    contentItem: Item {
        visible: !root.concealed
        BasicLabel {
            anchors.fill: parent
            opacity: 1.0
            wrapMode: Text.Wrap
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font: root.font
            text: root.text
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

        SvgColorImage {
            visible: root.concealed
            anchors.fill: parent
            anchors.margins: root.padding
            iconWidth: Style.roundToIconSize(width)
            iconHeight: Style.roundToIconSize(height)
            aspectRatio: Qt.KeepAspectRatio
            icon: MaterialIcons.eyeOffOutline
            color: root.foregroundColor
        }
    }
}
