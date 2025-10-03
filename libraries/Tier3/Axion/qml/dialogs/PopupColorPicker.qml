pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion

BasicPopup {
    id: root

    signal colorChosen(color color)

    required property AbstractItemModel colorModel
    required property color currentColor

    property int columns: 4
    property int squareSize: 40
    property int squareRadius: 4
    property int squareSpacing: 2
    property string instructions: qsTr("Choisissez une couleur")

    onColorChosen: (color) => {
        currentColor = color
        close()
    }

    contentItem: ColumnLayout {
        BasicLabel {
            Layout.alignment: Qt.AlignHCenter
            font: root.font
            text: root.instructions
        }

        Grid {
            Layout.preferredWidth: root.squareSize*root.columns + root.squareSpacing*(root.columns-1)
            Layout.minimumHeight: root.squareSize*root.columns + root.squareSpacing*(root.columns-1)

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
                    readonly property bool highlighted: color===root.currentColor

                    border.width: root.squareSpacing
                    border.color: highlighted ? Style.colorAccent : colorRect.color
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
}
