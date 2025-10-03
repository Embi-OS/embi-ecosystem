pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion

Item {
    id: root

    implicitWidth: calendarView.implicitWidth
    implicitHeight: calendarView.implicitHeight

    readonly property date today: new Date()

    property bool tumblerEnabled: false

    property alias locale: calendar.locale

    property alias month: calendar.month
    property alias year: calendar.year

    property alias from: calendar.from
    property alias to: calendar.to
    property alias date: calendar.selectedDate

    readonly property int selectedDay: root.date.getDate()
    readonly property int selectedMonth: root.date.getMonth()
    readonly property int selectedYear: root.date.getFullYear()

    property alias dayOfWeekVisible: calendar.dayOfWeekVisible
    property alias weekNumberVisible: calendar.weekNumberVisible

    signal dayClicked(date date)

    ColumnLayout {
        id: calendarView
        anchors.fill: parent
        spacing: 0

        CalendarNavigationBar {
            id: navigationBar
            Layout.fillWidth: true

            month: calendar.month
            year: calendar.year

            previousMonthEnabled: loader.status!==Loader.Ready
            nextMonthEnabled: loader.status!==Loader.Ready

            onPreviousMonth: calendar.previousMonth()
            onNextMonth: calendar.nextMonth()

            onYearClicked: {
                if(root.tumblerEnabled)
                    return;
                if(loader.sourceComponent===yearPicker)
                    loader.sourceComponent = undefined
                else
                    loader.sourceComponent = yearPicker
            }
            onMonthClicked: {
                if(root.tumblerEnabled)
                    return;
                if(loader.sourceComponent===monthPicker)
                    loader.sourceComponent = undefined
                else
                    loader.sourceComponent = monthPicker
            }
            onDateClicked: {
                if(!root.tumblerEnabled)
                    return;
                if(loader.sourceComponent===datePicker)
                    loader.sourceComponent = undefined
                else
                    loader.sourceComponent = datePicker
            }
        }

        CalendarGrid {
            id: calendar
            Layout.fillWidth: true
            Layout.fillHeight: true

            visible: loader.status!==Loader.Ready
            selectedDate: root.today

            onDayClicked: (date) => {
                root.date = date;
                root.dayClicked(date);
            }
        }

        Loader {
            id: loader
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: loader.status===Loader.Ready
        }
    }

    Component {
        id: datePicker
        DatePickerTumbler {
            //selectedDate: root.date
            Binding on selectedDate { value: root.date; delayed: true }

            from: root.from
            to: root.to
            onAccepted: (date) => {
                root.date = date;
                root.dayClicked(date);
            }
        }
    }

    Component {
        id: yearPicker
        CalendarYearPickerView {
            //year: root.year
            Binding on year { value: root.year; delayed: true }

            from: root.from.getFullYear()
            to: root.to.getFullYear()
            onAccepted: (year) => {
                if(year>=root.from.getFullYear() && year<=root.to.getFullYear())
                    root.year = year
                loader.sourceComponent = undefined
            }
        }
    }

    Component {
        id: monthPicker
        CalendarMonthPickerView {
            //month: root.month
            Binding on month { value: root.month; delayed: true }

            onAccepted: (month) => {
                if(month>=root.from.getMonth() && month<=root.to.getMonth())
                    root.month = month
                loader.sourceComponent = undefined
            }
        }
    }
}
