import QtQuick
import QtCharts
import Eco.Tier3.Axion

BasicXYGraphView {
    id: root

    property alias zoomControl: zoomControl
    property alias background: backgroundCanvas
    property alias foreground: foregroundCanvas

    property bool zoomEnabled: false
    property bool canvasEnabled: false

    onGraphChanged: root.zoomControl.resetZoom()
    onRefreshed: {
        if(!root.canvasEnabled)
            return;
        backgroundCanvas.requestPaint();
        foregroundCanvas.requestPaint();
    }

    XYGraphViewCanvas {
        id: foregroundCanvas
        view: root
        visible: root.canvasEnabled

        labelFont: root.labelFont
        labelFontColor: root.labelFontColor
        axisColor: root.axisColor
    }

    XYGraphViewCanvas {
        id: backgroundCanvas
        view: root
        visible: root.canvasEnabled

        labelFont: root.labelFont
        labelFontColor: root.labelFontColor
        axisColor: root.axisColor
    }

    XYGraphViewZoom {
        id: zoomControl
        view: root
        zoomEnabled: root.zoomEnabled
    }
}
