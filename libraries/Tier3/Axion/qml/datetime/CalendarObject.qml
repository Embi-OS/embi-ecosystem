pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion

QtObject {
    id: root

    property var locale: Qt.locale()

    property date selectedDate: new Date()
    property int month: selectedDate.getMonth()
    property int year: selectedDate.getFullYear()
    property alias from: calendarModel.from
    property alias to: calendarModel.to

    property bool dayOfWeekVisible: false
    property bool weekNumberVisible: false

    readonly property CalendarModel calendarModel: CalendarModel {
        id: calendarModel
        from: new Date(1900, 0, 1)
        to: new Date(2100, 11, 31)
    }

    onSelectedDateChanged: {
        root.month = selectedDate.getMonth()
        root.year = selectedDate.getFullYear()
    }

    function previousMonth() {
        if (root.month) {
            if (root.isMonthYearValid(from, to, root.month - 1, root.year))
                root.month--;
        } else {
            if (root.isMonthYearValid(from, to, 11, root.year - 1)) {
                root.month = 11;
                root.year--;
            }
        }
    }

    function nextMonth() {
        if (root.month < 11) {
            if (root.isMonthYearValid(from, to, root.month + 1, root.year))
                root.month++;
        } else {
            if (root.isMonthYearValid(from, to, 0, root.year + 1)) {
                root.month = 0;
                root.year++;
            }
        }
    }

    function isMonthYearValid(from: date, to: date, month: int, year: int): bool {
        if (isNaN(from) && isNaN(to))
            return true
        if (isNaN(from))
            return new Date(year, month, 1) <= to
        if (isNaN(to))
            return from <= new Date(year, month, new Date(year, month + 1, 0).getDate())
        return (from <= new Date(year, month, new Date(year, month + 1, 0).getDate())) && (new Date(year, month, 1) <= to)
    }
}
