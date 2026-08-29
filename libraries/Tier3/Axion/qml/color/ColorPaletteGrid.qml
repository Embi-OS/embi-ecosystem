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

    signal colorChosen(color color)

    required property AbstractItemModel colorModel
    required property color currentColor

    property int columns: 8
    property int squareSize: 32
    property int squareRadius: 4
    property int squareBorder: Style.containerBorderWidth
    property int squareSpacing: 4
    readonly property int count: repeater.count

    contentItem: Grid {
        columns: root.columns
        columnSpacing: root.squareSpacing
        rowSpacing: root.squareSpacing

        Repeater {
            id: repeater
            model: root.colorModel

            delegate: Rectangle {
                id: colorRect
                implicitWidth: root.squareSize
                implicitHeight: root.squareSize

                required color
                readonly property bool highlighted: ColorUtils.name(root.currentColor, true)===ColorUtils.name(color, true)

                border.width: highlighted ? 2 : root.squareBorder
                border.color: highlighted ? Style.colorAccent : Style.colorPrimaryLight
                radius: root.squareRadius

                SvgColorImage {
                    anchors.centerIn: parent
                    visible: colorRect.highlighted
                    size: 24
                    color: ColorUtils.isDarkColor(colorRect.color) ? Style.colorWhite : Style.colorBlack
                    icon: MaterialIcons.check
                }

                TapHandler {
                    onTapped: root.colorChosen(colorRect.color)
                }
            }
        }
    }
}