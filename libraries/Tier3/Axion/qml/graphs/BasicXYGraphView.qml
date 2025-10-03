import QtQuick
import QtCharts
import Eco.Tier1.Utils
import Eco.Tier3.Axion

ChartView {
    id: root

    property alias xAxis: xAxis
    property alias yAxis: yAxis

    required property GraphObject graph
    readonly property GraphAxisObject xAxisObject: graph ? graph.xAxis : null
    readonly property GraphAxisObject yAxisObject: graph ? graph.yAxis : null

    property string xUnitLabel: xAxisObject ? xAxisObject.unitLabel : "N/A"
    property double xAxisLowerBound: xAxisObject ? xAxisObject.lowerBound : 0
    property double xAxisUpperBound: xAxisObject ? xAxisObject.upperBound : 1

    property string yUnitLabel: yAxisObject ? yAxisObject.unitLabel : "N/A"
    property double yAxisLowerBound: yAxisObject ? yAxisObject.lowerBound : 0
    property double yAxisUpperBound: yAxisObject ? yAxisObject.upperBound : 1

    property font labelFont: Style.textTheme.body1
    property color labelFontColor: Style.colorWhite
    property color axisColor: Style.colorWhite

    property int refreshInhibitTime: 32

    signal rangeChanged()
    signal pointAdded()
    signal refreshed()

    onRangeChanged: {
        root.refresh()
    }
    onPointAdded: {
        root.refresh()
    }
    onPlotAreaChanged: {
        root.refresh()
    }
    onGraphChanged: {
        root.refresh()
    }

    function refresh() {
        throttler.throttle()
    }

    Connections {
        enabled: root.visible
        target: root.graph
        function onPointAdded() {
            root.pointAdded()
        }
    }

    SignalTrailingThrottler {
        id: throttler
        timeout: root.refreshInhibitTime
        onTriggered: {
            if(root.graph && root.graph.useSeries) {
                for(var i=0; i<root.graph.series.size; i++) {
                    var seriesObject = root.graph.series.at(i) as GraphSeriesObject;
                    if(!seriesObject.visible)
                        continue
                    if(i>=root.count)
                        root.createSeries(seriesObject.type, seriesObject.name, root.xAxis, root.yAxis)
                    var series = root.series(i) as AbstractSeries;
                    seriesObject.updateChartSeries(series);
                }
            }

            root.refreshed();
        }
    }

    margins.top: 0
    margins.bottom: 0
    margins.left: 0
    margins.right: 0

    antialiasing: true // false
    backgroundColor: "transparent"
    legend.visible: false

    ValuesAxis {
        id: xAxis

        min: root.xAxisLowerBound
        max: root.xAxisUpperBound

        onRangeChanged: root.rangeChanged()

        tickCount: 2
        color: root.axisColor
        labelsVisible: true
        labelsColor: root.labelFontColor
        labelsFont: root.labelFont
        titleText: ("[%1]").arg(root.xUnitLabel)
        titleFont: root.labelFont
        titleBrush: root.axisColor
    }

    ValuesAxis {
        id: yAxis

        min: root.yAxisLowerBound
        max: root.yAxisUpperBound

        onRangeChanged: root.rangeChanged()

        tickCount: 2
        color: root.axisColor
        labelsVisible: true
        labelsColor: root.labelFontColor
        labelsFont: root.labelFont
        titleText: ("[%1]").arg(root.yUnitLabel)
        titleFont: root.labelFont
        titleBrush: root.axisColor
    }
}
