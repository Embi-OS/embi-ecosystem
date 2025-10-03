import QtQuick
import QtCharts
import Eco.Tier3.Axion

Item {
    id: root

    property BasicXYGraphView view
    parent: root.view

    property bool zoomEnabled: true
    onZoomEnabledChanged: {
        resetZoom();
    }

    x: root.view.plotArea.x
    y: root.view.plotArea.y
    width: root.view.plotArea.width
    height: root.view.plotArea.height

    function resetZoom() {
        root.view.zoomReset();

        root.view.xAxis.min = Qt.binding(() => root.view.xAxisLowerBound);
        root.view.xAxis.max = Qt.binding(() => root.view.xAxisUpperBound);
        root.view.yAxis.min = Qt.binding(() => root.view.yAxisLowerBound);
        root.view.yAxis.max = Qt.binding(() => root.view.yAxisUpperBound);
    }

    function zoomAt(widthFactor: real, heightFactor: real, shiftX: real, shiftY: real) {
        var center = Qt.point(root.view.plotArea.x+root.view.plotArea.width/2, root.view.plotArea.y+root.view.plotArea.height/2)
        var width = root.view.plotArea.width/widthFactor
        var height = root.view.plotArea.height/heightFactor
        var posX = (center.x-width/2)+shiftX
        var posY = (center.y-height/2)+shiftY

        var rect = Qt.rect(posX,posY,width,height)

        root.view.zoomIn(rect);
    }

    PinchArea {
        id: pinchArea
        anchors.fill: root

        onPinchUpdated: (pinch) => {
            if(!root.zoomEnabled)
                return;

            var shiftX = (pinch.previousCenter.x-pinch.center.x);
            var shiftY = (pinch.previousCenter.y-pinch.center.y);
            var factor = 1-(pinch.previousScale-pinch.scale);

            root.zoomAt(factor, factor, shiftX, shiftY);
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            enabled: root.enabled

            property point previousPosition: Qt.point(0,0)

            onWheel: (wheel) => {
                if(!root.zoomEnabled)
                    return;

                var factor = 1+((wheel.angleDelta.y/8)/180);

                root.zoomAt(factor, factor, 0, 0);
            }

            onDoubleClicked: {
                root.resetZoom();
            }
            onPositionChanged: (mouse) => {
                if(!root.zoomEnabled)
                    return;

                if(mouseArea.previousPosition.x!==0 && mouseArea.previousPosition.y!==0)
                {
                    var shiftX = (mouseArea.previousPosition.x-mouse.x);
                    var shiftY = (mouseArea.previousPosition.y-mouse.y);

                    root.view.scrollRight(shiftX);
                    root.view.scrollDown(shiftY);
                }

                mouseArea.previousPosition = Qt.point(mouse.x, mouse.y)
            }
            onReleased: {
                mouseArea.previousPosition = Qt.point(0,0);
            }
        }
    }

}
