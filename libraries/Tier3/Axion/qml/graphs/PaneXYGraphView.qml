import QtQuick
import QtCharts
import Eco.Tier3.Axion

BasicPane {
    id: root

    drawFrame: true
    color: Style.colorPrimary
    padding: 0

    property alias chartView: chartView

    required property GraphObject graph

    title: ""
    comment: ""
    property color labelColor: Style.colorWhite
    property string label: ""
    property string caption: ""

    property bool zoomEnabled: false
    property bool canvasEnabled: isCompleted
    property bool running: false

    contentItem: XYGraphView {
        id: chartView
        graph: root.graph
        zoomEnabled: root.zoomEnabled
        canvasEnabled: root.canvasEnabled
    }

    header: RowContainer {
        topInset: root.headerPadding
        leftInset: root.headerPadding
        rightInset: root.headerPadding

        LabelWithCaption {
            Layout.fillWidth: true
            visible: haveAText || haveACaption

            text: root.title
            textFont: Style.textTheme.title1

            caption: root.comment
            captionFont: Style.textTheme.subtitle1
        }

        LabelWithCaption {
            visible: haveAText || haveACaption
            horizontalAlignment: Qt.AlignRight

            textColor: root.labelColor
            text: root.label
            textFont: Style.textTheme.title1

            caption: root.caption
            captionFont: Style.textTheme.body1
            captionMaximumLineCount: 0
        }

        BasicBusyIndicator {
            Layout.alignment: Qt.AlignVCenter
            visible: root.running
        }

        BasicToolButton {
            id: xScopedButton
            highlighted: checked
            icon.source: MaterialIcons.arrowExpandHorizontal
        }

        BasicToolButton {
            id: yScopedButton
            highlighted: checked
            icon.source: MaterialIcons.arrowExpandVertical
        }
    }

    Binding {
        when: chartView.graph
        target: chartView.graph
        property: "xAxis.lowerScoped"
        value: xScopedButton.checked
    }
    Binding {
        when: chartView.graph
        target: chartView.graph
        property: "xAxis.upperScoped"
        value: xScopedButton.checked
    }
    Binding {
        when: chartView.graph
        target: chartView.graph
        property: "yAxis.lowerScoped"
        value: yScopedButton.checked
    }
    Binding {
        when: chartView.graph
        target: chartView.graph
        property: "yAxis.upperScoped"
        value: yScopedButton.checked
    }
}
