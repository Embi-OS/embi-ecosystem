import QtQuick
import Eco.Tier3.Axion
import Eco.Tier3.Media

RowLayout {
    id: root

    required property MediaPlayer mediaPlayer
    readonly property int totalDuration: mediaPlayer.duration
    readonly property int currentPosition: mediaPlayer.position
    property alias isMediaSliderPressed: mediaSlider.pressed

    property double buttonSize: 32

    function getTime(time: int, withHour: bool): string {
        const h = FormatUtils.intToString(Math.floor(time / 3600000),2)
        const m = FormatUtils.intToString(Math.floor(time / 60000 - h * 60),2)
        const s = FormatUtils.intToString(Math.floor(time / 1000 - m * 60 - h * 3600),2)
        if(!withHour)
            return `${m}:${s}`
        return `${h}:${m}:${s}`
    }

    BasicLabel {
        Layout.alignment: Qt.AlignVCenter
        font: Style.textTheme.title2
        text: root.getTime(root.currentPosition, root.totalDuration>=3600000)
    }

    BasicSlider {
        id: mediaSlider
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignVCenter
        enabled: root.mediaPlayer.seekable
        size: root.buttonSize/4
        to: 1.0
        value: root.currentPosition / root.totalDuration
        hint: root.getTime(value * root.totalDuration, root.totalDuration>=3600000)

        onMoved: root.mediaPlayer.setPosition(value * root.totalDuration)
    }

    BasicLabel {
        Layout.alignment: Qt.AlignVCenter
        font: Style.textTheme.title2
        text: root.getTime(root.totalDuration, root.totalDuration>=3600000)
    }
}
