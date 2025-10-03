pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion

BasicDialog {
    id: root

    signal alarmValidated(var alarmMap)

    title: qsTr("Planification")
    property string buttonReject: qsTr("Annuler")
    property string buttonAccept: qsTr("Valider")

    locale: Qt.locale()

    buttonsContainer: [
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.RejectRole; text: root.buttonReject },
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole; enabled: root.isValid; text: root.buttonAccept; highlighted: true }
    ]

    property alias hour: timePicker.selectedHour
    property alias minute: timePicker.selectedMinute
    property date date: DateTimeUtils.currentDateTime()
    property alias name: nameField.text
    property alias repeat: alarmRepeat.checked
    property int weekdays: 0

    property bool showRepeat: true
    property bool showName: true

    readonly property date dateSelected: new Date(root.date.getFullYear(),root.date.getMonth(),root.date.getDate(),root.hour,root.minute,0)
    readonly property bool isValid: (root.repeat ? root.weekdays : DateTimeUtils.isDateAfter(dateSelected,DateTimeUtils.systemDateTime))
    property bool dateAuto: false

    onAccepted: {
        root.alarmValidated({
            "hour": root.hour,
            "minute": root.minute,
            "date": root.repeat ? "" : root.date,
            "enabled": true,
            "name": root.showName ? root.name : "",
            "repeat": root.showRepeat ? root.repeat : false,
            "weekdays": root.repeat ? root.weekdays : 0,
        })
    }

    preferredHeight: 0
    preferredWidth: 0

    topPadding: Style.contentRectangleBorderWidth
    bottomPadding: Style.contentRectangleBorderWidth

    function evaluateDate(date: date): string {
        var now = new Date();
        var selectedTime = new Date(date.getFullYear(),date.getMonth(),date.getDate())
        var today = new Date();
        today.setHours(0,0,0,0);

        if(!DateTimeUtils.isDateAfter(selectedTime, today)) {
            root.dateAuto=true
            selectedTime = today;
        }

        if(root.dateAuto && (root.hour*60+root.minute)>(now.getHours()*60+now.getMinutes())) {
            selectedTime = new Date(today.getFullYear(),today.getMonth(),today.getDate(),root.hour,root.minute,0)
        }
        else if(DateTimeUtils.isDateBefore(selectedTime,DateTimeUtils.systemDateTime)) {
            root.dateAuto=true
            selectedTime = new Date(today.getFullYear(),today.getMonth(),today.getDate()+1,root.hour,root.minute,0)
        }

        root.date = selectedTime
    }

    contentItem: Flickable {
        implicitHeight: layout.implicitHeight
        implicitWidth: layout.implicitWidth
        contentHeight: Math.max(height, layout.height)
        flickableDirection: Flickable.AutoFlickIfNeeded
        clip: true

        ColumnLayout {
            id: layout
            spacing: Style.contentRectangleBorderWidth
            width: parent.width

            TimePickerTumbler {
                id: timePicker
                Layout.alignment: Qt.AlignHCenter
                showSeconds: false
                onAccepted: root.evaluateDate(root.date)
            }

            RowLayout {
                Layout.fillWidth: true

                BasicCheckBox {
                    id: alarmRepeat
                    Layout.alignment: Qt.AlignVCenter
                    visible: root.showRepeat
                    font: Style.textTheme.button
                    text: qsTr("Répéter")
                }

                LayoutSpring {}

                TextButton {
                    Layout.alignment: Qt.AlignVCenter
                    visible: !alarmRepeat.checked
                    text: DateTimeUtils.formatRelativeDate(root.date, Locale.ShortFormat)
                    checkable: true
                    round: true
                    flat: true
                    onClicked: {
                        DialogManager.showDate({
                            selectedDate: root.date,
                            onDateSelected: function(date) {
                                root.dateAuto=false
                                root.evaluateDate(date)
                            }
                        });
                    }
                }
            }

            DayOfWeekRow {
                id: dayOfWeek
                visible: alarmRepeat.checked && alarmRepeat.visible
                Layout.fillWidth: true
                locale: root.locale
                topPadding: 0

                delegate: CalendarDateButton {
                    required property string shortName
                    required property int index

                    text: shortName
                    highlighted: MathUtils.bitTest(root.weekdays,index)
                    outlined: highlighted
                    onClicked: root.weekdays = MathUtils.bitToggle(root.weekdays,index)
                }
            }

            FormTextField {
                id: nameField
                visible: root.showName
                Layout.fillWidth: true
                placeholder: qsTr("Nom")
            }
        }
    }
}
