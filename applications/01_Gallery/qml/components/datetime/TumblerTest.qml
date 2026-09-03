import QtQuick
import Eco.Tier3.Axion

Item {
    id: root

    property date selectedTime: new Date()

    implicitWidth: Math.min(720, parent ? parent.width : 720)
    implicitHeight: Math.min(layout.implicitHeight, parent ? parent.height : layout.implicitHeight)

    function formatTime(time: date): string {
        return FormatUtils.intToString(time.getHours(), 2)
                + ":" + FormatUtils.intToString(time.getMinutes(), 2)
                + ":" + FormatUtils.intToString(time.getSeconds(), 2)
    }

    ColumnLayout {
        id: layout
        width: root.width
        spacing: Style.dialogSpacing

        BasicLabel {
            Layout.fillWidth: true
            text: qsTr("Tumbler gesture test")
            font: Style.textTheme.title1
        }

        BasicLabel {
            Layout.fillWidth: true
            wrapMode: Text.Wrap
            text: qsTr("Keep the pointer pressed, then drag it outside the outlined tumbler area. The movement state should remain active until release.")
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Style.dialogSpacing

            Item {
                Layout.preferredWidth: directTumbler.implicitWidth
                Layout.preferredHeight: directTumbler.implicitHeight

                Rectangle {
                    anchors.fill: directTumbler
                    color: Style.colorTransparent
                    border.color: directTumbler.moving ? Style.colorAccent : Style.colorPrimaryLight
                    border.width: Style.contentRectangleBorderWidth
                    radius: Style.contentRectangleRadius
                }

                BasicTumbler {
                    id: directTumbler
                    anchors.centerIn: parent
                    model: 24
                    currentIndex: 12
                    wrap: wrapCheckBox.checked
                }
            }

            ColumnLayout {
                Layout.fillWidth: true

                BasicCheckBox {
                    id: wrapCheckBox
                    text: qsTr("Wrap")
                    checked: true
                }

                BasicLabel {
                    text: qsTr("Current index: %1").arg(directTumbler.currentIndex)
                }

                BasicLabel {
                    text: directTumbler.moving ? qsTr("Moving") : qsTr("Stopped")
                    color: directTumbler.moving ? Style.colorAccent : Style.colorPrimaryLight
                }
            }
        }

        BasicSeparator {
            orientation: Qt.Horizontal
        }

        BasicLabel {
            Layout.fillWidth: true
            text: qsTr("TimePickerTumbler")
            font: Style.textTheme.subtitle1
        }

        TimePickerTumbler {
            id: timePicker
            Layout.fillWidth: true
            showSeconds: true
            selectedTime: root.selectedTime
            onTimeChanged: (time) => root.selectedTime = time
        }

        BasicLabel {
            text: qsTr("Selected time: %1").arg(root.formatTime(root.selectedTime))
        }

        RawButton {
            text: qsTr("Open DialogTimePicker")
            icon.source: MaterialIcons.clockOutline
            highlighted: true
            onClicked: timeDialog.open()
        }
    }

    DialogTimePicker {
        id: timeDialog
        selectedTime: root.selectedTime
        showSeconds: true
        onTimeSelected: (time) => root.selectedTime = time
    }
}
