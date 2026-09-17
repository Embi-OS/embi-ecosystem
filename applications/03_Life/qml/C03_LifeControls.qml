pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import L03_Life

Pane {
    id: root
    objectName: "lifeControls"

    required property GameOfLifeModel model
    required property int cellSize
    property string errorMessage
    signal cellSizeEdited(int value)

    padding: 12

    contentItem: ColumnLayout {
        spacing: 8

        Flow {
            Layout.fillWidth: true
            spacing: 8

            Button {
                objectName: "playButton"
                text: root.model.running ? qsTr("Pause") : qsTr("Play")
                highlighted: true
                onClicked: root.model.running = !root.model.running
            }

            Button {
                text: qsTr("Next generation")
                enabled: !root.model.running
                onClicked: root.model.nextStep()
            }

            Button {
                text: qsTr("Restart")
                enabled: root.model.generation > 0
                onClicked: root.model.restart()
            }

            Button {
                text: qsTr("Clear")
                onClicked: {
                    root.model.running = false
                    root.model.clear()
                    root.errorMessage = ""
                }
            }

            Button {
                text: qsTr("Gosper glider gun")
                onClicked: {
                    root.model.running = false
                    root.errorMessage = root.model.loadFile(":/cells/gosperglidergun.cells")
                        ? "" : qsTr("The pattern could not be loaded.")
                }
            }

            Button {
                text: qsTr("Randomize")
                onClicked: {
                    root.model.running = false
                    root.model.randomize()
                    root.errorMessage = ""
                }
            }
        }

        Flow {
            Layout.fillWidth: true
            spacing: 16

            RowLayout {
                Label { text: qsTr("Cell size") }
                SpinBox {
                    editable: true
                    value: root.cellSize
                    from: 1
                    to: 20
                    Accessible.name: qsTr("Cell size in pixels")
                    onValueModified: root.cellSizeEdited(value)
                }
            }

            RowLayout {
                Label { text: qsTr("Speed (gen/s)") }
                SpinBox {
                    editable: true
                    from: 1
                    to: 240
                    value: root.model.stepsPerSecond
                    Accessible.name: qsTr("Target generations per second")
                    onValueModified: root.model.stepsPerSecond = value
                }
            }
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Text.Wrap
            text: qsTr("Generation %1 · Alive %2 / %3 · %4 × %5 · Actual %6 gen/s")
                .arg(root.model.generation).arg(root.model.alive).arg(root.model.total)
                .arg(root.model.width).arg(root.model.height)
                .arg(Math.round(root.model.actualStepsPerSecond))
        }

        Label {
            Layout.fillWidth: true
            wrapMode: Text.Wrap
            text: qsTr("Left drag: draw · Right drag: erase · Arrows: move selection · Space: toggle cell · Tab: controls")
        }

        Label {
            Layout.fillWidth: true
            visible: root.errorMessage.length > 0
            wrapMode: Text.Wrap
            text: root.errorMessage
        }
    }
}
