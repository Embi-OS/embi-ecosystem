pragma ComponentBehavior: Bound
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
    spacing: swatchSpacing

    signal leftClicked()
    signal rightClicked()

    property int swatchSize: 32
    property int swatchRadius: 4
    property int swatchBorder: Style.containerBorderWidth
    property int swatchSpacing: 4

    property color leftColor: Style.colorAccent
    property color rightColor: Style.colorAccent
    property bool alphaEnabled: true

    component Swatch: Rectangle {
        id: swatch

        implicitWidth: Style.buttonMinWidth
        implicitHeight: root.swatchSize

        Layout.fillWidth: true
        Layout.fillHeight: true
        radius: root.swatchRadius

        border.width: root.swatchBorder
        border.color: Style.colorPrimaryLight

        BasicLabel {
            anchors.centerIn: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            relativeBackgroundColor: swatch.color
            font: Style.textTheme.subtitle1
            text: ColorUtils.name(swatch.color, root.alphaEnabled)
        }
    }

    contentItem: RowLayout {
        spacing: root.spacing

        Swatch {
            Layout.fillWidth: true
            Layout.fillHeight: true

            color: root.leftColor

            TapHandler {
                onTapped: root.leftClicked()
            }
        }

        Swatch {
            Layout.fillWidth: true
            Layout.fillHeight: true

            color: root.rightColor

            TapHandler {
                onTapped: root.rightClicked()
            }
        }
    }
}
