import QtQuick
import Eco.Tier3.Axion

RowContainer {
    id: root

    property date fromDate: new Date()
    readonly property string fromDateValue: Qt.formatDate(fromDate,"yyyy-MM-dd")
    property date toDate: new Date()
    readonly property string toDateValue: Qt.formatDate(toDate,"yyyy-MM-dd")

    function reset() {
        root.fromDate = new Date()
        root.toDate = new Date()
    }

    BasicLabel {
        text: qsTr("De:")
        verticalAlignment: Text.AlignVCenter
        font: Style.textTheme.headline5
    }

    TextButton {
        text: DateTimeUtils.formatDate(root.fromDate,Locale.ShortFormat)
        onClicked: {
            DialogManager.showDate({
                "selectedDate": root.fromDate,
                "onDateSelected": function(date) {
                    root.fromDate=date;
                }
            });
        }
    }

    BasicLabel {
        text: qsTr("à:")
        verticalAlignment: Text.AlignVCenter
        font: Style.textTheme.headline5
    }

    TextButton {
        text: DateTimeUtils.formatDate(root.toDate,Locale.ShortFormat)
        onClicked: {
            DialogManager.showDate({
                "selectedDate": root.toDate,
                "onDateSelected": function(date) {
                    root.toDate=date;
                }
            });
        }
    }
}
