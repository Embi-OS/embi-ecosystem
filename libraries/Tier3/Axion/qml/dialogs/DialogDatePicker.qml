import QtQuick
import Eco.Tier3.Axion

BasicDialog {
    id: root

    signal dateSelected(date date)

    locale: Qt.locale()
    property alias selectedDate: calendar.date
    property alias dayOfWeekVisible: calendar.dayOfWeekVisible
    property alias weekNumberVisible: calendar.weekNumberVisible

    property string buttonReject: qsTr("Annuler")
    property string buttonAccept: qsTr("Valider")

    function printDate(date: date): string {
        var day = root.locale.dayName(date.getDay())
        var dateString = Qt.formatDate(date, root.locale, Locale.ShortFormat)

        return day+", "+dateString
    }

    function resetDate() {
        root.selectedDate = new Date()
    }

    title: printDate(root.selectedDate)
    headerHorizontalAlignment: Qt.AlignHCenter
    headerVerticalAlignment: Qt.AlignVCenter
    drawHeaderSeparator: true

    onAccepted: root.dateSelected(root.selectedDate)

    buttonsContainer: [
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole; text: root.buttonAccept; highlighted: true},
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.RejectRole; text: root.buttonReject},
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.ActionRole; text: qsTr("Aujourd'hui"); onClicked: root.resetDate()}
    ]

    preferredHeight: 0
    preferredWidth: 0

    padding: Style.contentRectangleBorderWidth
    topPadding: undefined

    contentItem: DatePicker {
        id: calendar

        locale: root.locale
        dayOfWeekVisible: true
        tumblerEnabled: true

        onDayClicked: (date) => root.selectedDate = date
    }
}
