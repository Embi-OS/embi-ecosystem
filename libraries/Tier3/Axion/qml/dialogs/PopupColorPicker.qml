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

        ColorPaletteGrid {
            Layout.preferredWidth: squareSize*columns + squareSpacing*(columns-1)
            Layout.minimumHeight: squareSize*columns + squareSpacing*(columns-1)

            inset: 0
            padding: 0
            columns: root.columns
            squareSize: root.squareSize
            squareRadius: root.squareRadius
            squareSpacing: root.squareSpacing

            colorModel: root.colorModel
            currentColor: root.currentColor

            onColorChosen: (color) => root.colorChosen(color)
        }
    }
}
