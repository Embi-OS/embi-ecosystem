import QtQuick
import Eco.Tier3.Axion

CalendarNavigationBar {
    id: root

    readonly property date date: new Date()

    month: root.date.getMonth()
    year: root.date.getFullYear()

    onNextMonth: console.log("Next Clicked")
    onPreviousMonth: console.log("Previous Clicked")
    onMonthClicked: console.log("Month Clicked")
    onYearClicked: console.log("Year Clicked")
}
