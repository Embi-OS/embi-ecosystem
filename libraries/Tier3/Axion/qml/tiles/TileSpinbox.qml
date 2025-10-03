pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.SpinBox {
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
    topPadding: padding + (up.indicator ? up.indicator.height : 0) + (title.text!=="" ? title.height : 0)
    bottomPadding: padding + (down.indicator ? down.indicator.height : 0)

    font: Style.textTheme.body2

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

    property double elevation: flat ? 0 : Style.tileElevation

    property string text

    inputMethodHints: Qt.ImhFormattedNumbersOnly
    validator: IntegerValidator {
        bottom: Math.min(root.from, root.to)
        top: Math.max(root.from, root.to)
    }

    contentItem: TextInput {
        z: 2
        text: root.displayText
        clip: width < implicitWidth
        padding: 6

        font: Style.textTheme.headline4
        color: root.foregroundColor
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter

        readOnly: !root.editable
        validator: root.validator
        inputMethodHints: root.inputMethodHints
    }

    up.indicator: Item {
        x: (root.width - width)/2
        y: root.topInset + root.padding + (title.text!=="" ? title.height : 0)
        implicitWidth: upIcon.width
        implicitHeight: upIcon.height
        opacity: enabled ? 1.0 : 0.5

        SvgColorImage {
            id: upIcon
            anchors.centerIn: parent
            size: 32
            icon: MaterialIcons.arrowBigUp
            color: Style.colorWhite
        }

        BasicVeil {
            z: 100
            anchors.fill: parent
            radius: 4

            relativeBackgroundColor: root.backgroundColor
            focussed: root.up.pressed || root.up.hovered
            pressed: root.up.pressed
        }
    }

    down.indicator: Item {
        x: (root.width - width)/2
        y: root.height - height - (root.bottomInset + root.padding)
        implicitWidth: downIcon.width
        implicitHeight: downIcon.height
        opacity: enabled ? 1.0 : 0.5

        SvgColorImage {
            id: downIcon
            anchors.centerIn: parent
            size: 32
            icon: MaterialIcons.arrowBigDown
            color: Style.colorWhite
        }

        BasicVeil {
            z: 100
            anchors.fill: parent
            radius: 4

            relativeBackgroundColor: root.backgroundColor
            focussed: root.down.pressed || root.down.hovered
            pressed: root.down.pressed
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

        BasicLabel {
            id: title
            opacity: 1.0
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: root.padding-root.topInset
            horizontalAlignment: Text.AlignHCenter
            font: root.font
            text: root.text
            color: root.foregroundColor
            wrapMode: Text.Wrap
        }
    }
}
