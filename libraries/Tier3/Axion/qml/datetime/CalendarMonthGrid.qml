pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion

GridLayout {
    id: root

    signal dayClicked(date date)

    property date selectedDate: new Date()

    property alias locale: monthGrid.locale
    property alias monthGrid: monthGrid

    property alias dayOfWeekVisible: dayOfWeekRow.visible
    property alias weekNumberVisible: weekNumberColumn.visible

    columns: 2
    rows: 2

    DayOfWeekRow {
        id: dayOfWeekRow
        visible: true
        locale: monthGrid.locale
        spacing: 0

        Layout.column: 1
        Layout.fillWidth: true

        delegate: BasicLabel {
            required property string shortName
            text: shortName
            font: Style.textTheme.subtitle2
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    WeekNumberColumn {
        id: weekNumberColumn
        visible: false

        month: monthGrid.month
        year: monthGrid.year
        locale: monthGrid.locale
        spacing: 0

        Layout.row: 1
        Layout.fillHeight: true

        delegate: BasicLabel {
            required property int weekNumber
            text: weekNumber
            font: Style.textTheme.subtitle2
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    MonthGrid {
        id: monthGrid
        spacing: 0

        Layout.column: 1
        Layout.row: 1
        Layout.fillWidth: true
        Layout.fillHeight: true

        delegate: CalendarDateButton {
            Layout.fillWidth: true
            Layout.fillHeight: true

            required property int year
            required property int month
            required property int day
            required property date date
            required property bool today
            opacity: month===monthGrid.month ? 1.0 : 0.5
            text: day
            highlighted: today
            outlined: DateTimeUtils.isDateEqual(date, root.selectedDate)
            onClicked: root.dayClicked(date)
        }

        contentItem: GridLayout {
            rows: 6
            columns: 7
            rowSpacing: monthGrid.spacing
            columnSpacing: monthGrid.spacing

            Repeater {
                model: monthGrid.source
                delegate: monthGrid.delegate
            }
        }
    }
}
