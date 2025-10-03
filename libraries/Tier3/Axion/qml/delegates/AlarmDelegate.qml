pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion

BasicItemDelegate {
    id: root

    property int hour: 0
    property int minute: 0
    property date date: DateTimeUtils.currentDateTime()
    property string name: ""
    property bool repeat: false
    property int weekdays: 0
    property string nextTimeRing: ""
    property bool switchEnableChecked: true

    property alias switchEnable: switchEnable

    property int checkState: Qt.Unchecked

    readonly property date validDate: DateTimeUtils.nextValidDateTimeForTime(root.hour,root.minute,0,0)

    locale: Qt.locale()

    contentItem: RowLayout {
        spacing: root.spacing

        BasicCheckIndicator {
            Layout.alignment: Qt.AlignVCenter
            visible: root.checkable
            checkState: root.checkState
        }

        ColumnLayout {
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
            Layout.fillHeight: true

            BasicLabel {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                opacity: enabled ? 1.0 : 0.5
                enabled: switchEnable.checked && !root.checkable
                font: Style.textTheme.subtitle2
                text: root.name
                visible: text!==""
            }

            LabelWithCaption {
                Layout.fillWidth: true
                enabled: switchEnable.checked && !root.checkable
                text: FormatUtils.intToString(root.hour,2)+":"+FormatUtils.intToString(root.minute,2)
                textFont: Style.textTheme.title1
                textColor: root.foregroundColor
                caption: root.nextTimeRing
                captionFont: Style.textTheme.hint1
                captionColor: ColorUtils.transparent(root.foregroundColor, 0.5)
                haveACaption: switchEnable.checked
            }
        }

        DayOfWeekRow {
            id: dayOfWeek
            visible: root.repeat
            enabled: switchEnable.checked && !root.checkable
            locale: root.locale
            spacing: 20
            Layout.alignment: Qt.AlignVCenter
            Layout.minimumHeight: 20
            Layout.preferredWidth: 200
            delegate: BasicLabel {
                required property int index
                required property string narrowName

                font: Style.textTheme.subtitle1
                text: narrowName
                opacity: enabled ? 1.0 : 0.5
                color: MathUtils.bitTest(root.weekdays, index) ? Style.colorAccent : Style.colorWhite
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        BasicLabel {
            visible: !root.repeat
            text: DateTimeUtils.formatRelativeDate(root.validDate, Locale.ShortFormat)
            opacity: enabled ? 1.0 : 0.5
            enabled: switchEnable.checked && !root.checkable
            Layout.alignment: Qt.AlignVCenter
            Layout.fillHeight: true
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
        }

        BasicSwitch {
            id: switchEnable
            enabled: root.clickable
            visible: !root.checkable
            checked: root.switchEnableChecked
        }
    }
}
