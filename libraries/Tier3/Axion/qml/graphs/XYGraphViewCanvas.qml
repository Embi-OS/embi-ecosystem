import QtQuick
import QtCharts
import Eco.Tier3.Axion

Canvas {
    id: root

    property font labelFont: Style.textTheme.body1
    property color labelFontColor: Style.colorWhite
    property color axisColor: Style.colorWhite

    property var drawCanvas: function() {}

    required property BasicXYGraphView view
    parent: view
    anchors.fill: view
    contextType: "2d"

    onAvailableChanged: {
        if(available) {
            var ctx = getContext("2d");
            ctx.font = ('%1 %2px %3').arg(root.labelFont.weight).arg(root.labelFont.pixelSize).arg(root.labelFont.family);
        }
    }

    onPaint: {
        if(!available)
            return;

        var ctx = getContext("2d");
        ctx.clearRect(0,0,width,height);
        root.drawCanvas();
    }

    function drawYTick(y: real, color: color, lineWidth: int, style: string, label: bool) {
        if(y<=root.view.yAxis.min || y>=root.view.yAxis.max)
            return;

        drawLine(Qt.point(root.view.xAxis.min, y), Qt.point(root.view.xAxis.max, y), color, lineWidth, style);

        if(label)
            drawYLabel(y, FormatUtils.realToString(y,1), root.axisColor);
    }

    function drawXTick(x: real, color: color, lineWidth: int, style: string, label: bool) {
        if(x<=root.view.xAxis.min || x>=root.view.xAxis.max)
            return;

        drawLine(Qt.point(x, root.view.yAxis.min), Qt.point(x, root.view.yAxis.max), color, lineWidth, style)

        if(label)
            drawXLabel(x, FormatUtils.realToString(x,1), root.axisColor);
    }

    function drawBox(start: point, end: point, color: color, lineWidth: int, style: string) {
        if(end.x<=root.view.xAxis.min || start.x>=root.view.xAxis.max)
            return;
        if(end.y<=root.view.yAxis.min || start.y>=root.view.yAxis.max)
            return;

        const pointStart = Qt.point(Math.max(start.x,root.view.xAxis.min),Math.max(start.y,root.view.yAxis.min));
        const pointEnd = Qt.point(Math.min(end.x,root.view.xAxis.max),Math.min(end.y,root.view.yAxis.max));

        if(start.x>root.view.xAxis.min) // Left
            drawLine(pointStart, Qt.point(pointStart.x, pointEnd.y), color, lineWidth, style);
        if(end.y<root.view.yAxis.max) // Top
            drawLine(Qt.point(pointStart.x, pointEnd.y), pointEnd, color, lineWidth, style);
        if(end.x<root.view.xAxis.max) // Right
            drawLine(pointEnd, Qt.point(pointEnd.x, pointStart.y), color, lineWidth, style);
        if(start.y>root.view.yAxis.min) // Bottom
            drawLine(Qt.point(pointEnd.x, pointStart.y), pointStart, color, lineWidth, style);
    }

    function drawXPit(point: point, color: color) {
        if(point.x<=root.view.xAxis.min || point.x>=root.view.xAxis.max ||
           point.y<=root.view.yAxis.min || point.y>=root.view.yAxis.max)
            return;

        drawLine(Qt.point(point.x, root.view.yAxis.min), point, color, 1, "")

        drawXLabel(point.x, FormatUtils.realToString(point.x,1), color);

        drawTrianglePoint(point, color, 180, 10)
    }

    function drawXPeak(point: point, color: color) {
        if(point.x<=root.view.xAxis.min || point.x>=root.view.xAxis.max ||
           point.y<=root.view.yAxis.min || point.y>=root.view.yAxis.max)
            return;

        drawLine(Qt.point(point.x, root.view.yAxis.min), point, color, 1, "")

        drawXLabel(point.x, FormatUtils.realToString(point.x,1), color);

        drawTrianglePoint(point, color, 0, 10)
    }

    function drawXPoint(point: point, color: color) {
        if(point.x<=root.view.xAxis.min || point.x>=root.view.xAxis.max ||
           point.y<=root.view.yAxis.min || point.y>=root.view.yAxis.max)
            return;

        drawLine(Qt.point(point.x, root.view.yAxis.min), point, color, 1, "")

        drawXLabel(point.x, FormatUtils.realToString(point.x,1), color);

        drawSquarePoint(point, color, 10);
    }

    function drawYPit(point: point, color: color) {
        if(point.x<=root.view.xAxis.min || point.x>=root.view.xAxis.max ||
           point.y<=root.view.yAxis.min || point.y>=root.view.yAxis.max)
            return;

        drawLine(point, Qt.point(root.view.xAxis.min, point.y), color, 1, "")

        drawYLabel(point.y, FormatUtils.realToString(point.y,1), color);

        drawTrianglePoint(point, color, 270, 10)
    }

    function drawYPeak(point: point, color: color) {
        if(point.x<=root.view.xAxis.min || point.x>=root.view.xAxis.max ||
           point.y<=root.view.yAxis.min || point.y>=root.view.yAxis.max)
            return;

        drawLine(point, Qt.point(root.view.xAxis.min, point.y), color, 1, "")

        drawYLabel(point.y, FormatUtils.realToString(point.y,1), color);

        drawTrianglePoint(point, color, 90, 10)
    }

    function drawYPoint(point: point, color: color) {
        if(point.x<=root.view.xAxis.min || point.x>=root.view.xAxis.max ||
           point.y<=root.view.yAxis.min || point.y>=root.view.yAxis.max)
            return;

        drawLine(point, Qt.point(root.view.xAxis.min, point.y), color, 1, "")

        drawYLabel(point.y, FormatUtils.realToString(point.y,1), color);

        drawSquarePoint(point, color, 10);
    }

    function drawYCachedZone(startY: real, endY: real, color: color) {
        drawCachedZone(Qt.point(root.view.xAxis.min, startY), Qt.point(root.view.xAxis.max, endY), color);
    }

    function drawXCachedZone(startX: real, endX: real, color: color) {
        drawCachedZone(Qt.point(startX, root.view.yAxis.min), Qt.point(endX, root.view.yAxis.max), color);
    }

    function drawYLabel(y: real, text: string, color: color) {
        if(y<=root.view.yAxis.min || y>=root.view.yAxis.max)
            return;

        var ctx = root.context
        const yy = root.view.mapToPosition(Qt.point(0,y)).y;

        ctx.save();
        ctx.strokeStyle = color;
        ctx.fillStyle = color
        ctx.translate(root.view.plotArea.x, yy);
        ctx.textAlign = "right";
        ctx.fillText(text, -5, root.labelFont.pixelSize/3);
        ctx.restore();
    }

    function drawXLabel(x: real, text: string, color: color) {
        if(x<=root.view.xAxis.min || x>=root.view.xAxis.max)
            return;

        var ctx = root.context
        const xx = root.view.mapToPosition(Qt.point(x,0)).x;

        ctx.save();
        ctx.strokeStyle = color;
        ctx.fillStyle = color
        ctx.translate(xx, root.view.plotArea.y+root.view.plotArea.height);
        ctx.textAlign = "center";
        ctx.fillText(text, 0, root.labelFont.pixelSize);
        ctx.restore();
    }

    function drawText(point: point, text: string, color: color) {
        if(point.x<=root.view.xAxis.min || point.x>=root.view.xAxis.max)
            return;
        if(point.y<=root.view.yAxis.min || point.y>=root.view.yAxis.max)
            return;

        var ctx = root.context
        const pos = root.view.mapToPosition(point);

        ctx.save();
        ctx.strokeStyle = color;
        ctx.fillStyle = color
        ctx.translate(pos.x, pos.y);
        ctx.textAlign = "center";
        ctx.fillText(text, 0, root.labelFont.pixelSize/3);
        ctx.restore();
    }

    function drawLine(start: point, end: point, color: color, lineWidth: int, style: string) {
        // TODO: faire un peu de trygonometrie pour les cas ou start ou end est en dehors
        var ctx = root.context
        const pointStart = root.view.mapToPosition(start);
        const pointEnd = root.view.mapToPosition(end);

        ctx.save();
        ctx.beginPath();

        if(style==="dash")
            ctx.setLineDash([5, 5])
        else if(style==="dot")
            ctx.setLineDash([2, 2])
        else
            ctx.setLineDash([1,0])

        ctx.lineWidth = lineWidth;
        ctx.strokeStyle = color;
        ctx.fillStyle = color;
        ctx.moveTo(pointStart.x,pointStart.y);
        ctx.lineTo(pointEnd.x,pointEnd.y);
        ctx.stroke();
        ctx.restore();
    }

    function drawCachedZone(start: point, end: point, color: color) {
        if(end.x<=root.view.xAxis.min || start.x>=root.view.xAxis.max)
            return;
        if(end.y<=root.view.yAxis.min || start.y>=root.view.yAxis.max)
            return;

        var ctx = root.context
        const pointStart = root.view.mapToPosition(Qt.point(Math.max(start.x,root.view.xAxis.min),Math.max(start.y,root.view.yAxis.min)));
        const pointEnd = root.view.mapToPosition(Qt.point(Math.min(end.x,root.view.xAxis.max),Math.min(end.y,root.view.yAxis.max)));

        ctx.save();
        //ctx.beginPath();
        ctx.strokeStyle = color;
        ctx.fillStyle = color;
        ctx.fillRect(pointStart.x, pointStart.y, pointEnd.x-pointStart.x, pointEnd.y-pointStart.y);
        ctx.stroke();
        ctx.restore();
    }

    function drawTrianglePoint(point: point, color: color, rotation: real, size: int) {
        if(point.x<=root.view.xAxis.min || point.x>=root.view.xAxis.max)
            return;
        if(point.y<=root.view.yAxis.min || point.y>=root.view.yAxis.max)
            return;

        var ctx = root.context
        const pos = root.view.mapToPosition(point);

        ctx.save();
        ctx.beginPath();
        ctx.strokeStyle = color;
        ctx.fillStyle = color
        ctx.translate(pos.x, pos.y);
        ctx.rotate(rotation*(Math.PI / 180))
        ctx.moveTo(0, 0);
        ctx.lineTo(-size, -size);
        ctx.lineTo(+size, -size);
        ctx.closePath();
        ctx.fill();
        ctx.restore();
    }

    function drawSquarePoint(point: point, color: color, size: int) {
        if(point.x<=root.view.xAxis.min || point.x>=root.view.xAxis.max)
            return;
        if(point.y<=root.view.yAxis.min || point.y>=root.view.yAxis.max)
            return;

        var ctx = root.context
        const pos = root.view.mapToPosition(point);

        ctx.save();
        ctx.beginPath();
        ctx.strokeStyle = color;
        ctx.fillStyle = color
        ctx.moveTo(pos.x-size/2, pos.y-size/2);
        ctx.lineTo(pos.x-size/2, pos.y+size/2);
        ctx.lineTo(pos.x+size/2, pos.y+size/2);
        ctx.lineTo(pos.x+size/2, pos.y-size/2);
        ctx.closePath();
        ctx.fill();
        ctx.restore();
    }

    function drawRoundPoint(point: point, color: color, size: int) {
        if(point.x<=root.view.xAxis.min || point.x>=root.view.xAxis.max)
            return;
        if(point.y<=root.view.yAxis.min || point.y>=root.view.yAxis.max)
            return;

        var ctx = root.context
        const pos = root.view.mapToPosition(point);

        ctx.save();
        ctx.beginPath();
        ctx.strokeStyle = color;
        ctx.fillStyle = color
        ctx.translate(pos.x, pos.y);
        ctx.arc(0, 0, size/2, 0, 360*(Math.PI / 180), true)
        ctx.closePath();
        ctx.fill();
        ctx.restore();
    }
}
