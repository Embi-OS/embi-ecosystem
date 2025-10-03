pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion

Item {
    id: root

    signal dayClicked(date date)

    // Réexposition des propriétés de calendarObject
    property alias locale: calendarObject.locale
    property alias selectedDate: calendarObject.selectedDate
    property alias month: calendarObject.month
    property alias year: calendarObject.year
    property alias from: calendarObject.from
    property alias to: calendarObject.to
    property alias dayOfWeekVisible: calendarObject.dayOfWeekVisible
    property alias weekNumberVisible: calendarObject.weekNumberVisible

    function previousMonth() {
        calendarObject.previousMonth()
    }

    function nextMonth() {
        calendarObject.nextMonth()
    }

    CalendarObject {
        id: calendarObject
    }

    implicitWidth: 280
    implicitHeight: 280

    CalendarMonthGrid {
        anchors.fill: parent

        monthGrid.month: root.month
        monthGrid.year: root.year

        locale: root.locale
        selectedDate: root.selectedDate

        onDayClicked: (date) => root.dayClicked(date)

        dayOfWeekVisible: root.dayOfWeekVisible
        weekNumberVisible: root.weekNumberVisible
    }
}
