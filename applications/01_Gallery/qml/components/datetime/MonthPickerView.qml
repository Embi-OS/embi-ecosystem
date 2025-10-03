import QtQuick
import Eco.Tier3.Axion

CalendarMonthPickerView {
    id: root

    readonly property date date: new Date()
    month: root.date.getMonth()

    onAccepted: (month) => console.log("Accept month", DateTimeUtils.monthName(month+1))
}
