pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

GridView {
    id: root

    property int year: new Date().getFullYear()
    property alias from: numberModel.from
    property alias to: numberModel.to

    readonly property int indexSelectedYear: year - from

    signal accepted(int year)

    clip: true
    reuseItems: true

    implicitWidth: 280
    implicitHeight: 280

    cellWidth: Math.min(width / (Math.floor(width / 60)), width)
    cellHeight: Math.min(height / (Math.floor(height / 40)), height)

    model: numberModel

    NumberModel {
        id: numberModel
        delayed: true
        from: 1900
        to: 2100
    }

    currentIndex: indexSelectedYear

    delegate: Item {
        id: internalItem
        implicitWidth: root.cellWidth
        implicitHeight: root.cellHeight

        required property int value

        CalendarDateButton {
            anchors.fill: parent

            text: internalItem.value

            highlighted: internalItem.value===root.year
            onClicked: {
                root.year = internalItem.value
                root.accepted(root.year)
            }
        }
    }
}
