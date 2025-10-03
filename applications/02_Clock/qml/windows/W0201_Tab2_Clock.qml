import QtQuick
import Eco.Tier3.Axion
import L02_Clock

Item {
    id: root

    NumericClock {
        id: clock
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -20
        implicitHeight: root.height/3
        size: root.height/3

        hour: DateTimeUtils.hour
        minute: DateTimeUtils.minute
        second: DateTimeUtils.second
        secondeVisible: true
    }

    BasicLabel {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 20
        anchors.top: clock.bottom
        text: Clock.alarmModel.nextTimeRing
    }

    RowLayout {
        id: controls
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10

        FabButton {
            font: Style.textTheme.title2
            visible: ClockDisplay.state===ClockDisplayStates.On
            highlighted: true
            text: qsTr("Veille")
            icon.source: MaterialIcons.sleep
            onClicked: ClockDisplay.gotoSleep()
        }
        FabButton {
            font: Style.textTheme.title2
            visible: Clock.ringing
            text: qsTr("Snooze")
            icon.source: MaterialIcons.alarmSnooze
            onClicked: Clock.snooze()
        }
        FabButton {
            font: Style.textTheme.title2
            visible: Clock.ringing || Clock.snoozed
            text: qsTr("Alarme Off")
            icon.source: MaterialIcons.alarmOff
            onClicked: Clock.stopAlarm()
        }
    }
}
