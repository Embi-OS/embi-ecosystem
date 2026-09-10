pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion

GridView {
    id: root

    property int minimumMonth: 0
    property int maximumMonth: 11
    property int month: new Date().getMonth()

    signal accepted(int month)

    clip: true
    reuseItems: true

    implicitWidth: 280
    implicitHeight: 280

    cellHeight: Math.min(height / (Math.floor(height / 35)), height)
    cellWidth: Math.min(width / (Math.floor(width / 100)), width)

    model: 12

    delegate: Item {
        id: itemDelegate
        implicitWidth: root.cellWidth
        implicitHeight: root.cellHeight

        required property int index

        CalendarDateButton {
            anchors.fill: parent

            text: DateTimeUtils.monthName(itemDelegate.index+1)

            enabled: itemDelegate.index >= root.minimumMonth && itemDelegate.index <= root.maximumMonth
            highlighted: itemDelegate.index===root.month
            onClicked: {
                if(!enabled)
                    return
                root.month = itemDelegate.index
                root.accepted(root.month)
            }
        }
    }
}
