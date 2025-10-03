import QtQuick
import Eco.Tier3.Axion

BasicTile {
    id: root

    property font tolFont: Style.textTheme.hint1
    property int tolDecimal: valueDecimal

    property bool tolMinEnable: true
    property double tolMin: 5.0

    property bool tolMaxEnable: true
    property double tolMax: 20.0

    property double value
    property string valueUnit: ""
    property int valueDecimal: 1
    property color valueColor: foregroundColor
    property font valueFont: Style.textTheme.title2
    property font captionFont: Style.textTheme.body2

    property bool showMarble: false
    property double marbleValue: 0
    property color marbleColor: (root.tolMinEnable && (root.marbleValue < root.tolMin)) ? Style.colorError :
                                (root.tolMaxEnable && (root.marbleValue > root.tolMax)) ? Style.colorFatal :
                                Style.colorSuccess

    property color colorGauge: (root.tolMinEnable && (root.value < root.tolMin)) ? Style.colorError :
                               (root.tolMaxEnable && (root.value > root.tolMax)) ? Style.colorFatal :
                               Style.colorSuccess

    contentItem: ColumnLayout {
        visible: !root.concealed
        spacing: root.spacing
        BasicLabel {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            opacity: 1.0
            font: root.font
            text: root.text
            color: root.foregroundColor
            wrapMode: Text.Wrap
        }

        LinearGaugeImpl {
            id: linearGauge
            Layout.fillWidth: true
            Layout.fillHeight: true

            readonly property double delta: root.tolMax-root.tolMin

            from: root.tolMin - (delta/100)*15
            to: root.tolMax + (delta/100)*15
            value: root.value
            lineWidth: Style.tileLineWidth
            spacing: root.spacing

            progressColor: root.colorGauge
            backgroundColor: Qt.darker(progressColor, 3.8)
            textColor: root.colorGauge
            text: FormatUtils.realToString(root.value, root.valueDecimal)
            textFont: root.valueFont
            tickFont: root.tolFont
            tickDecimal: root.tolDecimal

            showMarble: root.showMarble
            marbleValue: root.marbleValue
            marbleColor: root.marbleColor
            showCaption: root.showMarble
            captionColor: root.marbleColor
            caption: FormatUtils.realToString(root.marbleValue, root.valueDecimal)
            captionFont: root.captionFont

            function generateTicks(tolMin: double, tolMax: double): list<double> {
                let numbers = []
                if(root.tolMinEnable)
                    numbers.push(tolMin)
                if(root.tolMaxEnable)
                    numbers.push(tolMax)
                return numbers
            }
            ticks: (root.tolMinEnable || root.tolMaxEnable) ? generateTicks(root.tolMin, root.tolMax) : []

            Ruler {
                readonly property double topPoint: linearGauge.ticksPos.length>=1 ? linearGauge.ticksPos[0] : 0
                readonly property double bottomPoint: linearGauge.ticksPos.length>=2 ? linearGauge.ticksPos[1] : linearGauge.height
                anchors.right: linearGauge.horizontalCenter
                anchors.rightMargin: linearGauge.lineWidth/2 + root.spacing/2
                anchors.top: linearGauge.top
                anchors.topMargin: topPoint
                anchors.bottom: linearGauge.bottom
                anchors.bottomMargin: linearGauge.height-bottomPoint
                font: root.tolFont
                from: root.tolMin
                to: root.tolMax
                decimal: root.tolDecimal
            }
        }

        BasicLabel {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            opacity: 1.0
            font: root.font
            text: ("[%1]").arg(root.valueUnit)
            color: root.foregroundColor
        }
    }
}
