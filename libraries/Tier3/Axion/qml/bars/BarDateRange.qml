import QtQuick
import Eco.Tier3.Axion

RowContainer {
    id: root

    property date today: new Date()
    property date fromDate: today
    readonly property string fromDateValue: Qt.formatDate(fromDate,"yyyy-MM-dd")
    property date toDate: today
    readonly property string toDateValue: Qt.formatDate(toDate,"yyyy-MM-dd")

    function reset() {
        root.fromDate = today
        root.toDate = today
    }

    RawButton {
        icon.source: MaterialIcons.calendar
        text: DateTimeUtils.formatDateRange(root.fromDate, root.toDate, Locale.LongFormat)
        onClicked: {
            DialogManager.showDateRange({
                "selectedDate": root.toDate,
                "fromDate": root.fromDate,
                "toDate": root.toDate,
                "onDateRangeSelected": function(fromDate, toDate) {
                    root.fromDate=fromDate;
                    root.toDate=toDate;
                }
            });
        }
    }
}
