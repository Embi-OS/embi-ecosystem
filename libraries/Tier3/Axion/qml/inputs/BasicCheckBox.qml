import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.CheckBox {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                    Math.max(implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding))

    spacing: 8
    padding: 8

    font: Style.textTheme.button

    opacity: enabled ? 1.0 : 0.5
    Behavior on opacity { NumberAnimation { duration: 150 } }

    property color relativeBackgroundColor: Style.colorBackground
    property color foregroundColor: ColorUtils.isDarkColor(relativeBackgroundColor) ? Style.colorWhite : Style.colorBlack

    indicator: BasicCheckIndicator {
        x: root.text ? (root.mirrored ? root.width - width - root.rightPadding : root.leftPadding) : root.leftPadding + (root.availableWidth - width) / 2
        y: root.topPadding + (root.availableHeight - height) / 2
        checkState: root.checkState

        BasicVeil {
            z: -1
            anchors.centerIn: parent
            width: parent.width*2
            height: parent.height*2
            radius: height/2

            relativeBackgroundColor: root.relativeBackgroundColor
            focussed: root.pressed || root.hovered
            pressed: root.pressed
        }
    }

    contentItem: Text {
        leftPadding: root.indicator && !root.mirrored ? root.indicator.width + root.spacing : 0
        rightPadding: root.indicator && root.mirrored ? root.indicator.width + root.spacing : 0

        text: root.text
        font: root.font
        color: root.foregroundColor
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
    }
}
