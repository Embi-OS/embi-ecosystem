pragma ComponentBehavior: Bound
import QtQml
import QtQuick
import QtQuick.Controls.Material
import L03_Life

ApplicationWindow {
    id: root

    visibility: Window.Windowed
    width: 1280
    height: 800
    minimumWidth: 360
    minimumHeight: 480

    Material.theme: Material.Dark
    Material.background: root.backgroundColor
    Material.foreground: root.foregroundColor
    Material.primary: root.gridColor
    Material.accent: root.accentColor

    color: root.backgroundColor
    title: qsTr("Conway’s Game of Life")

    property int cellSize: 10
    readonly property int cellSpacing: Math.round(cellSize/10)

    readonly property color foregroundColor: "#FFFFFF"
    readonly property color gridColor: "#383642"
    readonly property color backgroundColor: "#121117"
    readonly property color accentColor: "#B93245"
    readonly property color cellColor: "#EB5967"

    GameOfLifeModel {
        id: gameOfLifeModel
        width: Math.max(0, Math.floor((gameOfLifeView.width - root.cellSpacing) / (root.cellSize + root.cellSpacing)))
        height: Math.max(0, Math.floor((gameOfLifeView.height - root.cellSpacing) / (root.cellSize + root.cellSpacing)))
    }

    GameOfLifeView {
        id: gameOfLifeView
        objectName: "boardView"
        property int selectedRow: 0
        property int selectedColumn: 0
        property bool selectedAlive: false
        Accessible.role: Accessible.CheckBox
        Accessible.name: qsTr("Cell row %1, column %2").arg(selectedRow + 1).arg(selectedColumn + 1)
        Accessible.description: qsTr("Arrow keys move; Space toggles the selected cell; Tab leaves the board.")
        Accessible.checked: selectedAlive
        Accessible.onToggleAction: toggleSelected()

        function toggleSelected(): void {
            gameOfLifeModel.running = false
            gameOfLifeModel.toggleValue(selectedRow, selectedColumn)
        }

        onCellPainted: (row, column) => {
            selectedRow = row
            selectedColumn = column
        }
        onSelectedRowChanged: selectedAlive = gameOfLifeModel.value(selectedRow, selectedColumn)
        onSelectedColumnChanged: selectedAlive = gameOfLifeModel.value(selectedRow, selectedColumn)
        Keys.onPressed: (event) => {
            switch (event.key) {
            case Qt.Key_Left: selectedColumn = Math.max(0, selectedColumn - 1); break
            case Qt.Key_Right: selectedColumn = Math.min(gameOfLifeModel.width - 1, selectedColumn + 1); break
            case Qt.Key_Up: selectedRow = Math.max(0, selectedRow - 1); break
            case Qt.Key_Down: selectedRow = Math.min(gameOfLifeModel.height - 1, selectedRow + 1); break
            case Qt.Key_Space: toggleSelected(); break
            default: return
            }
            event.accepted = true
        }

        Connections {
            target: gameOfLifeModel
            function onBoardChanged() {
                gameOfLifeView.selectedRow = Math.max(0, Math.min(gameOfLifeView.selectedRow, gameOfLifeModel.height - 1))
                gameOfLifeView.selectedColumn = Math.max(0, Math.min(gameOfLifeView.selectedColumn, gameOfLifeModel.width - 1))
                gameOfLifeView.selectedAlive = gameOfLifeModel.value(gameOfLifeView.selectedRow, gameOfLifeView.selectedColumn)
            }
        }

        Rectangle {
            id: selection
            x: Math.max(0, (gameOfLifeView.width - (gameOfLifeModel.width * (root.cellSize + root.cellSpacing) + root.cellSpacing)) / 2)
                + root.cellSpacing + gameOfLifeView.selectedColumn * (root.cellSize + root.cellSpacing)
            y: Math.max(0, (gameOfLifeView.height - (gameOfLifeModel.height * (root.cellSize + root.cellSpacing) + root.cellSpacing)) / 2)
                + root.cellSpacing + gameOfLifeView.selectedRow * (root.cellSize + root.cellSpacing)
            width: root.cellSize
            height: root.cellSize
            visible: gameOfLifeView.activeFocus && gameOfLifeModel.width > 0 && gameOfLifeModel.height > 0
            color: "transparent"
            border.color: root.foregroundColor
            border.width: 2
            Accessible.ignored: true
        }

        anchors.fill: parent
        anchors.margins: root.cellSpacing + 1
        model: gameOfLifeModel
        cellSize: root.cellSize
        cellSpacing: root.cellSpacing
        gridColor: root.gridColor
        backgroundColor: root.backgroundColor
        cellColor: root.cellColor
    }

    footer: C03_LifeControls {
        model: gameOfLifeModel
        cellSize: root.cellSize
        onCellSizeEdited: (value) => root.cellSize = value
    }

    onClosing: gameOfLifeModel.running = false
}
