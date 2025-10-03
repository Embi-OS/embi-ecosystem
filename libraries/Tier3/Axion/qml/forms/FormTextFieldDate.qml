import QtQuick
import Eco.Tier3.Axion

FormTextField {
    id: root

    signal dateAccepted(date date)

    property var date
    text: (root.validator as DateValidator).dateAsText(root.date)

    inputMethodHints: Qt.ImhFormattedNumbersOnly
    placeholder: "__/__/____"
    validator: DateValidator {}

    onAccepted: (text) => {
        root.dateAccepted((root.validator as DateValidator).textAsDate(text))
    }

    function showDateDialog() {
        var validDate = DateTimeUtils.isDateValid(root.date, (root.validator as DateValidator).from, (root.validator as DateValidator).to) ? root.date : DateTimeUtils.currentDate();
        DialogManager.showDate({
            selectedDate: validDate,
            onDateSelected: function(date) {
                root.date = date;
                root.dateAccepted(root.date);
                root.textInputFocus = false;
            }
        });
    }

    icons: [
        ClickableIcon {
            icon: MaterialIcons.calendar
            onClicked: root.showDateDialog()
        }
    ]
}
