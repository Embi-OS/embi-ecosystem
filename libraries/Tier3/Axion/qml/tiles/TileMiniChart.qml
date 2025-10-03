import QtQuick
import QtCharts
import Eco.Tier3.Axion

BasicTile {
    id: root

    contentItem: ChartView {
        id: chart
        visible: !root.concealed

        margins.top:0
        margins.bottom:0
        margins.left:0
        margins.right:0

        antialiasing: false
        backgroundColor: "transparent"
        legend.visible: false

        LineSeries {
            name: "LineSeries"
            XYPoint { x: 0; y: 0 }
            XYPoint { x: 1.1; y: 2.1 }
            XYPoint { x: 1.9; y: 3.3 }
            XYPoint { x: 2.1; y: 2.1 }
            XYPoint { x: 2.9; y: 4.9 }
            XYPoint { x: 3.4; y: 3.0 }
            XYPoint { x: 4.1; y: 3.3 }
        }
    }
}
