import QtQuick
import Eco.Tier3.Axion

RowLayout {
    id: root

    property alias showSeconds: secondTumbler.visible
    property bool wrap: true
    property int visibleItemCount: 3

    property date selectedTime: new Date()
    property date editedTime: selectedTime
    onSelectedTimeChanged: editedTime = selectedTime
    readonly property int selectedHour: hourTumbler.currentIndex
    readonly property int selectedMinute: minuteTumbler.currentIndex
    readonly property int selectedSecond: secondTumbler.currentIndex

    readonly property bool moving: hourTumbler.moving || minuteTumbler.moving || secondTumbler.moving

    signal timeChanged(date time)

    property bool _updatingSelectedTime: false

    function updateSelectedTime() {
        if(_updatingSelectedTime)
            return;
        _updatingSelectedTime = true;

        const seconds = root.showSeconds ? secondTumbler.currentIndex : 0;
        root.editedTime = new Date(0,0,0,hourTumbler.currentIndex, minuteTumbler.currentIndex, seconds);
        root.timeChanged(root.editedTime);

        _updatingSelectedTime = false;
    }

    Item {
        Layout.fillWidth: true
    }

    BasicTumbler {
        id: hourTumbler
        wrap: root.wrap
        visibleItemCount: root.visibleItemCount
        model: 24
        currentIndex: root.editedTime.getHours()
        delegate: LabelTumbler {
            required property int index
            text: FormatUtils.intToString(index,2)
        }
        onMovingChanged: {
            if(!moving)
                root.updateSelectedTime()
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
        currentIndex: root.editedTime.getMinutes()
        delegate: LabelTumbler {
            required property int index
            text: FormatUtils.intToString(index,2)
        }
        onMovingChanged: {
            if(!moving)
                root.updateSelectedTime()
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
        currentIndex: root.editedTime.getSeconds()
        delegate: LabelTumbler {
            required property int index
            text: FormatUtils.intToString(index,2)
        }
        onMovingChanged: {
            if(!moving)
                root.updateSelectedTime()
        }
    }

    Item {
        Layout.fillWidth: true
    }
}
