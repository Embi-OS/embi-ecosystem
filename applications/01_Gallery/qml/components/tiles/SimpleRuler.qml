import QtQuick
import Eco.Tier3.Axion

Item {
    id: root

    implicitWidth: Math.min(layout.width + 100, parent.width)
    implicitHeight: Math.min(500, parent.height)

    ColumnLayout {
        id: layout
        FormSlider {
            id: heightSlider
            implicitWidth: 600
            from: 200
            to: 500
            label: "height"
        }

        FormSlider {
            id: fromSlider
            implicitWidth: 600
            from: 0
            to: 50
            label: "from"
        }

        FormSlider {
            id: toSlider
            implicitWidth: 600
            from: 100
            to: 26000 //200
            label: "to"
        }
    }

    Ruler {
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        height: heightSlider.value
        from: fromSlider.value
        to: toSlider.value
        decimal: 1

        DebugRectangle {}
    }
}
