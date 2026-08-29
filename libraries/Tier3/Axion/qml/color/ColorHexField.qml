import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.Control {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    property real inset: 6
    topInset: inset
    bottomInset: inset
    leftInset: inset
    rightInset: inset
    padding: 6
    spacing: 4

    signal colorAccepted(color color)

    property color color: Style.colorAccent
    property bool alphaEnabled: true
    property int radius: 4

    contentItem: RowLayout {
        spacing: root.spacing

        Rectangle {
            Layout.fillHeight: true
            implicitWidth: height
            border.width: Style.containerBorderWidth
            border.color: Style.colorPrimaryLight
            color: root.color
            radius: root.radius
        }

        BasicTextField {
            id: textField
            Layout.fillWidth: true
            padding: 6
            focus: false

            font: Style.textTheme.headline5
            relativeBackgroundColor: Style.colorPrimary

            text: ColorUtils.name(root.color, root.alphaEnabled)

            validator: RegularExpressionValidator {
                regularExpression: root.alphaEnabled ? /^#?([0-9A-Fa-f]{6}|[0-9A-Fa-f]{8})$/ : /^#?[0-9A-Fa-f]{6}$/
            }

            background: Rectangle {
                implicitWidth: 128
                implicitHeight: Style.formCellHeight

                radius: root.radius
                color: "transparent"
                border.width: Style.formBorderWidth
                border.color: textField.activeFocus ? Style.colorAccent :
                              textField.acceptableInput ? Style.colorPrimaryLight : Style.colorWarning
            }

            onAccepted: root.colorAccepted(ColorUtils.fromName(text))
            onEditingFinished: root.colorAccepted(ColorUtils.fromName(text))
        }
    }
}
