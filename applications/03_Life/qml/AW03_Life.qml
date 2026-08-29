pragma ComponentBehavior: Bound
import QtQml
import QtQuick
import QtQuick.Window
import QtQuick.Controls.Material
import QtQuick.Layouts
import L03_Life

ApplicationWindow {
    id: root

    visibility: Window.Windowed
    width: 1280
    height: 800

    Material.theme: Material.Dark
    Material.background: backgroundColor
    Material.foreground: "#FFFFFF"
    Material.primary: gridColor
    Material.accent: cellColor

    color: backgroundColor
    title: qsTr("Conway’s Game of Life")

    property int cellSize: 10
    property int cellSpacing: Math.round(cellSize/10)
    onCellSpacingChanged: console.log(cellSpacing)

    readonly property color gridColor: "#383642"
    readonly property color backgroundColor: "#121117"
    readonly property color cellColor: "#EB5967"

    GameOfLifeView {
        id: gameOfLifeView
        anchors.fill: parent
        anchors.margins: root.cellSpacing+1

        cellSize: root.cellSize
        cellSpacing: root.cellSpacing
        gridColor: root.gridColor
        backgroundColor: root.backgroundColor
        cellColor: root.cellColor

        model: GameOfLifeModel {
            id: gameOfLifeModel
            width: Math.max(0, (gameOfLifeView.width-gameOfLifeView.cellSpacing)/(gameOfLifeView.cellSize + gameOfLifeView.cellSpacing))
            height: Math.max(0, (gameOfLifeView.height-gameOfLifeView.cellSpacing)/(gameOfLifeView.cellSize + gameOfLifeView.cellSpacing))
        }
    }

    footer: Rectangle {
        signal nextStep

        id: footer
        height: 50
        color: root.backgroundColor

        RowLayout {
            anchors.centerIn: parent

            SpinBox {
                editable: true
                value: root.cellSize
                from: 1
                to: 20
                onValueModified: root.cellSize = value
            }

            Button {
                text: qsTr("Clear")
                onClicked: gameOfLifeModel.clear()
            }

            Button {
                text: qsTr("Load")
                onClicked: gameOfLifeModel.loadFile(":/cells/gosperglidergun.cells");
            }

            Button {
                text: qsTr("Randomize")
                onClicked: gameOfLifeModel.randomize()
            }

            Button {
                text: qsTr("Next")
                onClicked: gameOfLifeModel.nextStep()
            }

            Slider {
                id: slider
                from: 1
                to: 240
                value: 60
                Layout.fillWidth: false
            }

            Button {
                text: timer.running ? "Pause" : "Play"
                onClicked: timer.running = !timer.running
            }
        }

        Timer {
            id: timer
            interval: 1000.0/slider.value
            running: false
            repeat: true

            onTriggered: gameOfLifeModel.nextStep()
        }
    }
}
