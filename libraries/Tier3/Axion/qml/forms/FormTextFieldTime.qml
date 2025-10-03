import QtQuick
import Eco.Tier3.Axion

FormTextField {
    id: root

    signal timeAccepted(date time)

    property var time
    text: (root.validator as TimeValidator).timeAsText(root.time)

    inputMethodHints: Qt.ImhFormattedNumbersOnly
    placeholder: "__:__"
    validator: TimeValidator {}

    onAccepted: (text) => {
        root.timeAccepted((root.validator as TimeValidator).textAsTime(text))
    }

    function showTimeDialog() {
        var validTime = DateTimeUtils.isTimeValid(root.time, (root.validator as TimeValidator).from, (root.validator as TimeValidator).to) ? root.time : DateTimeUtils.currentTime();
        DialogManager.showTime({
            selectedTime: validTime,
            onTimeSelected: function(time) {
                root.time = time;
                root.timeAccepted(root.time);
                root.textInputFocus = false;
            }
        });
    }

    icons: [
        ClickableIcon {
            icon: MaterialIcons.clock
            onClicked: root.showTimeDialog()
        }
    ]
}
