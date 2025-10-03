import QtQuick
import Eco.Tier3.Axion

BasicDialog {
    id: root

    signal timeSelected(date time)

    locale: Qt.locale()
    property alias selectedTime: time.selectedTime
    property alias showSeconds: time.showSeconds

    property string buttonReject: qsTr("Annuler")
    property string buttonAccept: qsTr("Valider")

    function resetTime() {
        var date = new Date()
        root.selectedTime = new Date(0,0,0,date.getHours(), date.getMinutes(), root.showSeconds ? date.getSeconds() : 0)
    }

    onAccepted: {
        root.timeSelected(new Date(0,0,0,root.selectedTime.getHours(), root.selectedTime.getMinutes(), root.showSeconds ? root.selectedTime.getSeconds() : 0))
    }

    onAboutToShow: {
        var date = new Date()
        root.selectedTime = new Date(0,0,0,date.getHours(), date.getMinutes(), root.showSeconds ? date.getSeconds() : 0)
    }

    buttonsContainer: [
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole; text: root.buttonAccept; highlighted: true},
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.RejectRole; text: root.buttonReject},
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.ActionRole; text: qsTr("Maintenant"); onClicked: root.resetTime()}
    ]

    preferredHeight: 0
    preferredWidth: 0

    padding: Style.contentRectangleBorderWidth
    topPadding: undefined

    contentItem: TimePickerTumbler {
        id: time

        onAccepted: (time) => root.selectedTime = time
    }
}
