import QtQuick
import Eco.Tier3.Axion
import Eco.Tier3.Media

RowLayout {
    id: root

    required property MediaPlayer mediaPlayer
    required property MediaItemModel mediaPlaylist
    required property bool isPlaylistVisible

    property double buttonSize: 40

    signal playlistClicked()

    ClickableIcon {
        id: playlistButton
        Layout.alignment: Qt.AlignVCenter
        size: root.buttonSize
        highlighted: root.isPlaylistVisible
        icon: MaterialIcons.playlistMusic
        onClicked: root.playlistClicked()
    }

    BasicLabel {
        Layout.alignment: Qt.AlignVCenter
        font: Style.textTheme.title2
        text: ("%1/%2").arg(root.mediaPlaylist.currentIndex+1).arg(root.mediaPlaylist.size)
    }

    Item {
        Layout.fillWidth: true
    }

    ClickableIcon {
        id: shuffleButton
        Layout.alignment: Qt.AlignVCenter
        size: root.buttonSize
        highlighted: root.mediaPlaylist.shuffled
        icon: MaterialIcons.shuffle
        onClicked: root.mediaPlaylist.shuffled = !root.mediaPlaylist.shuffled
    }

    ClickableIcon {
        id: loopButton
        Layout.alignment: Qt.AlignVCenter
        size: root.buttonSize
        icon: root.mediaPlaylist.playbackMode===MediaPlaybackModes.Sequential ? MaterialIcons.repeatOff :
              root.mediaPlaylist.playbackMode===MediaPlaybackModes.ItemLoop ? MaterialIcons.repeatOnce :
              root.mediaPlaylist.playbackMode===MediaPlaybackModes.Loop ? MaterialIcons.repeat : MaterialIcons.repeatOff
        onClicked: root.mediaPlaylist.changePlaybackMode()
    }

    ClickableIcon {
        id: rateButton
        Layout.alignment: Qt.AlignVCenter
        size: root.buttonSize
        highlighted: rateSlider.visible
        icon: rateSlider.value<=1.0 ? MaterialIcons.speedometerSlow :
              rateSlider.value<2.0 ? MaterialIcons.speedometerMedium : MaterialIcons.speedometer
        onClicked: rateSlider.open()

        PopupSlider {
            id: rateSlider
            ExtraPosition.position: ItemPositions.BottomEnd
            snapMode: Slider.SnapAlways
            from: 0.5
            to: 2.5
            stepSize: 0.5
            value: root.mediaPlayer.playbackRate
            onMoved: (value) => root.mediaPlayer.playbackRate = value
        }
    }
}
