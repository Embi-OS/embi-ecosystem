import QtQuick
import Eco.Tier3.Axion

RowLayout {
    id: root

    spacing: 20

    property double value
    SequentialAnimation on value {
        loops: Animation.Infinite
        NumberAnimation { to: 20; duration: 2000 }
        NumberAnimation { to: 0; duration: 2000 }
    }

    LinearGaugeImpl {
        from: 5
        to: 15
        value: root.value
        progressColor: Style.colorAccent
        backgroundColor: Qt.darker(progressColor, 3.8)
        text: FormatUtils.realToString(root.value, 1)
        textFont: Style.textTheme.title1
        tickFont: Style.textTheme.hint1
        ticks: [7, 13]

        showMarble: true
        marbleValue: 10
        marbleColor: "red"
        showCaption: true
        caption: FormatUtils.realToString(marbleValue, 1)
        captionFont: Style.textTheme.body1

        DebugRectangle {}
    }

    LinearGaugeImpl {
        from: 5
        to: 15
        value: root.value
        progressColor: Style.colorAccent
        backgroundColor: Qt.darker(progressColor, 3.8)
        text: FormatUtils.realToString(root.value, 1)
        textFont: Style.textTheme.title1
        tickFont: Style.textTheme.hint1
        ticks: [7, 13]

        DebugRectangle {}
    }

    LinearGaugeImpl {
        from: 5
        to: 15
        progressColor: Style.pink
        backgroundColor: Qt.darker(progressColor, 3.8)
        value: root.value
        capStyle: Qt.FlatCap
        text: FormatUtils.realToString(root.value, 1)
        textFont: Style.textTheme.title1

        showMarble: true
        marbleValue: 10
        marbleColor: "red"
        showCaption: true
        caption: FormatUtils.realToString(marbleValue, 1)
        captionFont: Style.textTheme.body1
    }

    LinearGaugeImpl {
        from: 5
        to: 15
        progressColor: Style.cyan
        backgroundColor: Qt.darker(progressColor, 3.8)
        value: root.value
        capStyle: Qt.SquareCap
        text: FormatUtils.realToString(root.value, 1)
        textFont: Style.textTheme.title1

        showMarble: true
        marbleValue: 10
        marbleColor: "red"
        showCaption: true
        caption: FormatUtils.realToString(marbleValue, 1)
        captionFont: Style.textTheme.body1
    }

    LinearGaugeImpl {
        from: 5
        to: 15
        progressColor: Style.green
        backgroundColor: Qt.darker(progressColor, 3.8)
        value: root.value
        lineWidth: 5
        text: FormatUtils.realToString(root.value, 1)
        textFont: Style.textTheme.title1

        showMarble: true
        marbleValue: 10
        marbleColor: "red"
        showCaption: true
        caption: FormatUtils.realToString(marbleValue, 1)
        captionFont: Style.textTheme.body1
    }
}
