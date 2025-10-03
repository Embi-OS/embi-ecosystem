import QtQuick
import Eco.Tier3.Axion

ColumnLayout {
    id: root

    CalendarMonthGrid {
        id: grid
        Layout.fillWidth: true
        Layout.fillHeight: true

        selectedDate: new Date()
        onDayClicked: (date) => console.log("Accept date", DateTimeUtils.formatDate(date))

        dayOfWeekVisible: dayOfWeekVisible.checked
        weekNumberVisible: weekNumberVisible.checked
    }

    BasicCheckBox {
        id: dayOfWeekVisible
        text: "dayOfWeekVisible"
    }

    BasicCheckBox {
        id: weekNumberVisible
        text: "weekNumberVisible"
    }

    BasicLabel { text: "date: " + DateTimeUtils.formatDate(grid.selectedDate) }
    BasicLabel { text: "month: " + DateTimeUtils.monthName(grid.monthGrid.month) }
    BasicLabel { text: "year: " + grid.monthGrid.year }
}
