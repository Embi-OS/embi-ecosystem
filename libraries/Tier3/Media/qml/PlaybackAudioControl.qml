import QtQuick
import Eco.Tier3.Axion
import Eco.Tier3.Media

RowLayout {
    id: root

    required property MediaPlayer mediaPlayer
    readonly property AudioOutput audioOutput: mediaPlayer.audioOutput

    property double buttonSize: 32

    SvgColorImage {
        size: root.buttonSize
        icon: MaterialIcons.volumeLow
        color: Style.colorWhite
    }

    BasicSlider {
        id: volumeSlider
        size: root.buttonSize/4
        to: 100.0
        value: root.audioOutput.volume * 100
        onMoved: root.audioOutput.volume = value / 100

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignVCenter
    }

    SvgColorImage {
        size: root.buttonSize
        icon: root.audioOutput.volume<0.5 ? MaterialIcons.volumeMedium : MaterialIcons.volumeHigh
        color: Style.colorWhite
    }
}
