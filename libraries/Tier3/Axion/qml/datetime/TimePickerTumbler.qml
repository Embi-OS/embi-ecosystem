import QtQuick
import Eco.Tier3.Axion

RowLayout {
    id: root

    property alias showSeconds: secondTumbler.visible
    property bool wrap: true
    property int visibleItemCount: 3

    property alias selectedHour: hourTumbler.currentIndex
    property alias selectedMinute: minuteTumbler.currentIndex
    property alias selectedSecond: secondTumbler.currentIndex

    property date selectedTime: new Date()

    signal accepted(date time)

    property bool _pendingEvaluate: true
    function evaluate() {
        if(_pendingEvaluate)
            return;
        _pendingEvaluate = true;

        root.selectedTime = new Date(0,0,0,hourTumbler.currentIndex, minuteTumbler.currentIndex, secondTumbler.currentIndex);
        root.accepted(root.selectedTime);

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
        id: hourTumbler
        wrap: root.wrap
        visibleItemCount: root.visibleItemCount
        model: 24
        currentIndex: root.selectedTime.getHours()
        delegate: LabelTumbler {
            required property int index
            text: FormatUtils.intToString(index,2)
        }
        onMovingChanged: {
            if(!moving)
                root.evaluate()
        }
    }

    LabelTumbler {
        text: ":"
        Layout.alignment: Qt.AlignVCenter
        opacity: 1.0
    }

    BasicTumbler {
        id: minuteTumbler
        wrap: root.wrap
        visibleItemCount: root.visibleItemCount
        model: 60
        currentIndex: root.selectedTime.getMinutes()
        delegate: LabelTumbler {
            required property int index
            text: FormatUtils.intToString(index,2)
        }
        onMovingChanged: {
            if(!moving)
                root.evaluate()
        }
    }

    LabelTumbler {
        text: ":"
        visible: secondTumbler.visible
        Layout.alignment: Qt.AlignVCenter
        opacity: 1.0
    }

    BasicTumbler {
        id: secondTumbler
        wrap: root.wrap
        visibleItemCount: root.visibleItemCount
        model: 60
        currentIndex: visible ? root.selectedTime.getSeconds() : -1
        delegate: LabelTumbler {
            required property int index
            text: FormatUtils.intToString(index,2)
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
