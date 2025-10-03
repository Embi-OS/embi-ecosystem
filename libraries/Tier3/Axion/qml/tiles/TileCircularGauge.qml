import QtQuick
import Eco.Tier3.Axion

BasicTile {
    id: root

    property font tolFont: Style.textTheme.hint1

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

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            RowLayout {
                anchors.fill: parent
                spacing: root.spacing

                BasicLabel {
                    id: tolMinLabel
                    Layout.alignment: Qt.AlignVCenter
                    Layout.minimumWidth: Math.max(tolMinLabel.contentWidth, tolMaxLabel.contentWidth)
                    horizontalAlignment: Text.AlignRight
                    opacity: 1.0
                    font: root.tolFont
                    text: root.tolMinEnable ? FormatUtils.realToString(root.tolMin,root.valueDecimal) : ""
                    color: root.foregroundColor
                }

                CircularGaugeImpl {
                    Layout.alignment: Qt.AlignVCenter
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    readonly property double delta: root.tolMax-root.tolMin

                    from: root.tolMin - (delta/180)*45
                    to: root.tolMax + (delta/180)*45
                    value: root.value
                    startAngle: 45
                    spanAngle: 270
                    lineWidth: Style.tileLineWidth

                    progressColor: root.colorGauge
                    backgroundColor: Qt.darker(progressColor, 3.8)
                    textColor: root.colorGauge
                    text: FormatUtils.realToString(root.value, root.valueDecimal)
                    textFont: root.valueFont

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
                }

                BasicLabel {
                    id: tolMaxLabel
                    Layout.alignment: Qt.AlignVCenter
                    Layout.minimumWidth: Math.max(tolMinLabel.contentWidth, tolMaxLabel.contentWidth)
                    horizontalAlignment: Text.AlignLeft
                    opacity: 1.0
                    font: root.tolFont
                    text: root.tolMaxEnable ? FormatUtils.realToString(root.tolMax,root.valueDecimal) : ""
                    color: root.foregroundColor
                }
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
