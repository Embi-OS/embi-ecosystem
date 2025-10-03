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

    property double marble
    SequentialAnimation on marble {
        loops: Animation.Infinite
        NumberAnimation { to: 11; duration: 2000 }
        NumberAnimation { to: 9; duration: 2000 }
    }

    CircularGaugeImpl {
        from: 5
        to: 15
        value: root.value
        progressColor: Style.colorAccent
        backgroundColor: Qt.darker(progressColor, 3.8)
        text: FormatUtils.realToString(root.value, 1)
        textFont: Style.textTheme.title2
        ticks: [7, 13]

        showMarble: true
        marbleValue: root.marble
        marbleColor: marbleValue>10?"red":"blue"
        showCaption: true
        caption: FormatUtils.realToString(marbleValue, 1)
        captionFont: Style.textTheme.body2
    }

    CircularGaugeImpl {
        from: 5
        to: 15
        progressColor: Style.pink
        backgroundColor: Qt.darker(progressColor, 3.8)
        value: root.value
        capStyle: Qt.FlatCap
        text: FormatUtils.realToString(root.value, 1)
        textFont: Style.textTheme.subtitle1

        showMarble: true
        marbleValue: root.marble
        marbleColor: marbleValue>10?"red":"blue"
        showCaption: true
        caption: FormatUtils.realToString(marbleValue, 1)
        captionFont: Style.textTheme.hint1
    }

    CircularGaugeImpl {
        from: 5
        to: 15
        progressColor: Style.cyan
        backgroundColor: Qt.darker(progressColor, 3.8)
        value: root.value
        capStyle: Qt.SquareCap
        text: FormatUtils.realToString(root.value, 1)
        textFont: Style.textTheme.title1

        showMarble: true
        marbleValue: root.marble
        marbleColor: marbleValue>10?"red":"blue"
        showCaption: true
        caption: FormatUtils.realToString(marbleValue, 1)
        captionFont: Style.textTheme.body1
    }

    CircularGaugeImpl {
        from: 5
        to: 15
        progressColor: Style.green
        backgroundColor: Qt.darker(progressColor, 3.8)
        value: root.value
        lineWidth: 5
        text: FormatUtils.realToString(root.value, 1)
        textFont: Style.textTheme.title1

        showMarble: true
        marbleValue: root.marble
        marbleColor: marbleValue>10?"red":"blue"
        showCaption: true
        caption: FormatUtils.realToString(marbleValue, 1)
        captionFont: Style.textTheme.body1
    }
}
