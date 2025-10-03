import QtQuick
import Eco.Tier3.Axion

ColumnLayout {
    id: root

    DatePicker {
        id: view
        Layout.fillWidth: true
        Layout.fillHeight: true

        date: new Date()
        onDayClicked: (date) => console.log("Accept date", date)

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

    BasicLabel { text: "date: " + DateTimeUtils.formatDate(view.date) }
    BasicLabel { text: "month: " + DateTimeUtils.monthName(view.month) }
    BasicLabel { text: "year: " + view.year }
    BasicLabel { text: "from: " + view.from }
    BasicLabel { text: "to: " + view.to }
}
