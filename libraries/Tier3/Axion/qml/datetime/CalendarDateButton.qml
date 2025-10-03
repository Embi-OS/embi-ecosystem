import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.AbstractButton {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    topInset: 1
    bottomInset: 1
    rightInset: 1
    leftInset: 1

    horizontalPadding: 10
    font: Style.textTheme.button

    property bool highlighted: checked
    property bool outlined: false

    readonly property color relativeBackgroundColor: root.backgroundColor.a>0 ? root.backgroundColor : Style.colorBackground
    readonly property color backgroundColor: root.checked ? Style.colorAccent : "transparent"
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

        radius: height / 2
        color: root.backgroundColor
        border.width: 2
        border.color: root.outlined ? Style.colorAccent : root.backgroundColor

        BasicVeil {
            z: 100
            anchors.fill: parent
            radius: parent.height / 2

            relativeBackgroundColor: root.relativeBackgroundColor
            focussed: enabled && (root.down || root.visualFocus || root.hovered)
            pressed: root.pressed
        }
    }
}
