pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion

ListView {
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

    model: calendarObject.calendarModel

    currentIndex: calendarObject.calendarModel.indexOf(calendarObject.year, calendarObject.month)
    cacheBuffer: 0
    clip: true
    reuseItems: true
    snapMode: ListView.SnapOneItem
    orientation: ListView.Horizontal
    highlightMoveDuration: 250
    highlightRangeMode: ListView.StrictlyEnforceRange
    spacing: 16
    interactive: false

    delegate: CalendarMonthGrid {
        required property int month
        required property int year

        width: ListView.view.width
        height: ListView.view.height

        monthGrid.month: month
        monthGrid.year: year

        locale: root.locale
        selectedDate: root.selectedDate

        onDayClicked: (date) => root.dayClicked(date)

        dayOfWeekVisible: root.dayOfWeekVisible
        weekNumberVisible: root.weekNumberVisible
    }
}
