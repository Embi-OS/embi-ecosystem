import QtQuick
import Eco.Tier3.Axion

CalendarYearPickerView {
    id: root

    readonly property date date: new Date()

    year: root.date.getFullYear()
    from: 1900
    to: 2100

    onAccepted: (year) => console.log("Accept year", year)
}
