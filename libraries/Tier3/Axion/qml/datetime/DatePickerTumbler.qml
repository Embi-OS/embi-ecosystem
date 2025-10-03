import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

RowLayout {
    id: root

    readonly property bool wrap: false
    property int visibleItemCount: 3

    property date from: new Date(2000, 0, 1)
    property date to: new Date(2050, 11, 31)

    property date selectedDate: new Date()

    readonly property int selectedDay: root.selectedDate.getDate()
    readonly property int selectedMonth: root.selectedDate.getMonth()
    readonly property int selectedYear: root.selectedDate.getFullYear()

    signal accepted(date date)

    property bool _pendingEvaluate: true
    function evaluate() {
        if(_pendingEvaluate)
            return;

        _pendingEvaluate = true;

        var newYear = yearTumbler.currentIndex+yearModel.from;
        var newMonth = monthTumbler.currentIndex;
        var maxDay = (new Date(newYear, newMonth+1, 0)).getDate();
        var newDay = dayTumbler.currentIndex+1;

        if(maxDay<newDay)
            newDay = maxDay;

        root.selectedDate = new Date(newYear, newMonth, newDay);

        dayTumbler.positionViewAtIndex(root.selectedDate.getDate()-1, ListView.Center)
        monthTumbler.positionViewAtIndex(root.selectedDate.getMonth(), ListView.Center)
        yearTumbler.positionViewAtIndex(root.selectedDate.getFullYear()-yearModel.from, ListView.Center)

        root.accepted(root.selectedDate);

        _pendingEvaluate = false;
    }

    Component.onCompleted: {
        _pendingEvaluate = false
        evaluate()
    }

    Item {
        Layout.fillWidth: true
    }

    BasicTumbler {
        id: dayTumbler
        wrap: root.wrap
        visibleItemCount: root.visibleItemCount
        currentIndex: root.selectedDate.getDate()-1
        model: NumberModel {
            id: dayModel
            from: 1
            to: (new Date(root.selectedDate.getFullYear(), root.selectedDate.getMonth() + 1, 0)).getDate()
        }
        delegate: LabelTumbler {
            required property int index
            readonly property int day: index+1
            text: FormatUtils.intToString(day,2)
        }
        onMovingChanged: {
            if(!moving)
                root.evaluate()
        }
    }

    BasicTumbler {
        id: monthTumbler
        wrap: root.wrap
        visibleItemCount: root.visibleItemCount
        currentIndex: root.selectedDate.getMonth()
        model: NumberModel {
            id: monthModel
            from: 0
            to: 11
        }
        delegate: LabelTumbler {
            required property int index
            required property int value
            text: DateTimeUtils.monthName(value+1, Locale.ShortFormat)
            horizontalAlignment: Text.AlignLeft
        }
        onMovingChanged: {
            if(!moving)
                root.evaluate()
        }
    }

    BasicTumbler {
        id: yearTumbler
        wrap: root.wrap
        visibleItemCount: root.visibleItemCount
        currentIndex: root.selectedDate.getFullYear()-yearModel.from
        model: NumberModel {
            id: yearModel
            from: root.from.getFullYear()
            to: root.to.getFullYear()
        }
        delegate: LabelTumbler {
            required property int index
            required property int value
            text: value
        }
        onMovingChanged: {
            if(!moving)
                root.evaluate()
        }
    }

    Item {
        Layout.fillWidth: true
    }
}
