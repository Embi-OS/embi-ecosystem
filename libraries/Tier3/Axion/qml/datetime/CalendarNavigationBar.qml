import QtQuick
import Eco.Tier3.Axion

RowLayout {
    id: root

    property int month
    property int year

    property alias previousMonthEnabled: previousMonthButton.enabled
    property alias nextMonthEnabled: nextMonthButton.enabled

    signal monthClicked()
    signal yearClicked()
    signal dateClicked()
    signal nextMonth()
    signal previousMonth()

    IconButton {
        id: previousMonthButton
        Layout.alignment: Qt.AlignVCenter

        flat: true
        round: true
        icon.source: MaterialIcons.chevronLeft

        onClicked: root.previousMonth()
    }

    LayoutSpring {}

    CalendarDateButton {
        id: monthButton
        Layout.alignment: Qt.AlignVCenter

        rightPadding: 4
        leftPadding: 4

        text: DateTimeUtils.monthName(root.month+1)
        onClicked: {
            root.monthClicked();
            root.dateClicked();
        }
    }

    CalendarDateButton {
        id: yearButton
        Layout.alignment: Qt.AlignVCenter

        rightPadding: 4
        leftPadding: 4

        text: root.year
        onClicked: {
            root.yearClicked();
            root.dateClicked();
        }
    }

    LayoutSpring {}

    IconButton {
        id: nextMonthButton
        Layout.alignment: Qt.AlignVCenter

        flat: true
        round: true
        icon.source: MaterialIcons.chevronRight

        onClicked: root.nextMonth()
    }
}
