import QtQuick
import Eco.Tier3.Axion

BasicDialog {
    id: root

    signal dateRangeSelected(date fromDate, date toDate)

    property alias selectedDate: calendar.date
    property alias fromDate: calendar.fromDate
    property alias toDate: calendar.toDate
    property alias dayOfWeekVisible: calendar.dayOfWeekVisible
    property alias weekNumberVisible: calendar.weekNumberVisible

    property string buttonReject: qsTr("Annuler")
    property string buttonAccept: qsTr("Valider")

    function today() {
        const today = DateTimeUtils.currentDate()
        root.selectedDate = today
    }

    function resetDate() {
        root.selectedDate = DateTimeUtils.currentDate()
        root.fromDate = root.selectedDate
        root.toDate = root.selectedDate
    }

    function thisWeek() {
        const today = DateTimeUtils.currentDate()
        const from = DateTimeUtils.date(today.getFullYear(), today.getMonth()+1, today.getDate() - (today.getDay()-1))
        const to = DateTimeUtils.dateTimeAddDays(from, 6)
        root.fromDate = from
        root.toDate = to
        root.selectedDate = today
    }

    function thisMonth() {
        const today = DateTimeUtils.currentDate()
        const from = DateTimeUtils.date(today.getFullYear(), today.getMonth()+1, 1)
        const to = DateTimeUtils.dateTimeAddDays(DateTimeUtils.date(today.getFullYear(), today.getMonth()+2, 1), -1)
        root.fromDate = from
        root.toDate = to
        root.selectedDate = today
    }

    function lastDays(count: int) {
        const today = DateTimeUtils.currentDate()
        const from = DateTimeUtils.dateTimeAddDays(today, -(count-1))
        root.fromDate = from
        root.toDate = today
        root.selectedDate = today
    }

    title: DateTimeUtils.formatDateRange(root.fromDate, root.toDate, Locale.LongFormat)
    headerHorizontalAlignment: Qt.AlignHCenter
    headerVerticalAlignment: Qt.AlignVCenter
    drawHeaderSeparator: true

    onAccepted: root.dateRangeSelected(root.fromDate, root.toDate)

    buttonsContainer: [
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole; text: root.buttonAccept; highlighted: true},
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.RejectRole; text: root.buttonReject},
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.ActionRole; text: qsTr("Aujourd'hui"); onClicked: root.today()}
    ]

    preferredHeight: 0
    preferredWidth: 0

    padding: Style.contentRectangleBorderWidth
    topPadding: undefined

    contentItem: Item {
        RowLayout {
            anchors.fill: parent

            ColumnLayout {
                Layout.fillHeight: true
                LayoutSpring {}
                CalendarDateButton { Layout.fillWidth: true; text: qsTr("Aujourd'hui"); onClicked: root.resetDate() }
                CalendarDateButton { Layout.fillWidth: true; text: qsTr("Cette semaine"); onClicked: root.thisWeek() }
                CalendarDateButton { Layout.fillWidth: true; text: qsTr("Ce mois"); onClicked: root.thisMonth() }
                CalendarDateButton { Layout.fillWidth: true; text: qsTr("7 derniers jours"); onClicked: root.lastDays(7) }
                CalendarDateButton { Layout.fillWidth: true; text: qsTr("14 derniers jours"); onClicked: root.lastDays(14) }
                CalendarDateButton { Layout.fillWidth: true; text: qsTr("30 derniers jours"); onClicked: root.lastDays(30) }
                LayoutSpring {}
            }

            BasicSeparator {
                color: Style.colorPrimaryLight
            }

            DatePicker {
                id: calendar
                implicitHeight: 300
                implicitWidth: 400
                Layout.fillWidth: true
                Layout.fillHeight: true

                locale: root.locale
                dayOfWeekVisible: true
                tumblerEnabled: false
                rangeMode: true

                onDayClicked: (date) => {
                    if (DateTimeUtils.isDateBefore(date, root.toDate) || !DateTimeUtils.isDateEqual(root.fromDate, root.toDate)) {
                        // Start a new range
                        root.fromDate = date
                        root.toDate = date
                    } else {
                        // Complete the range
                        root.toDate = date
                    }

                    // Update selectedDate to the most recent clicked date
                    root.selectedDate = date
                }
            }
        }
    }
}
