import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.AbstractButton {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    horizontalPadding: 10
    font: Style.textTheme.button

    property bool highlighted: checked
    property bool outlined: false
    property bool inRange: false
    property bool firstInRange: false
    property bool lastInRange: false
    property double radius: height / 2

    readonly property color relativeBackgroundColor: root.backgroundColor.a>0 ? root.backgroundColor : Style.colorBackground
    readonly property color backgroundColor: root.checked ? Style.colorAccent : root.inRange ? Style.colorAccentFade : "transparent"
    readonly property color foreroundColor: {
        if(root.highlighted && !root.checked)
            return Style.colorAccent
        return ColorUtils.isDarkColor(root.relativeBackgroundColor) ? Style.colorWhite : Style.colorBlack
    }

    contentItem: Text {
        text: root.text
        font: root.font

        opacity: enabled ? 1.0 : 0.5
        color: root.foreroundColor

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        implicitWidth: Style.buttonMinWidth
        implicitHeight: Style.buttonMinHeight

        topLeftRadius: root.inRange ? root.firstInRange ? root.radius : 0 : root.radius
        bottomLeftRadius: root.inRange ? root.firstInRange ? root.radius : 0 : root.radius
        topRightRadius: root.inRange ? root.lastInRange ? root.radius : 0 : root.radius
        bottomRightRadius: root.inRange ? root.lastInRange ? root.radius : 0 : root.radius

        color: root.backgroundColor

        BasicVeil {
            z: 100
            anchors.fill: parent
            anchors.margins: 1
            radius: height / 2

            visible: true
            border.width: 2
            border.color: root.outlined ? Style.colorAccent : color

            relativeBackgroundColor: root.relativeBackgroundColor
            focussed: enabled && (root.down || root.visualFocus || root.hovered)
            pressed: root.pressed
        }
    }
}
