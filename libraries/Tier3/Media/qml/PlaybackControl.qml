import QtQuick
import Eco.Tier3.Axion
import Eco.Tier3.Media

RowLayout {
    id: root

    required property MediaPlayer mediaPlayer
    readonly property int mediaPlayerState: root.mediaPlayer.playbackState

    signal playNextFile()
    signal playPreviousFile()

    property double buttonSize: 64

    Item {
        Layout.fillWidth: true
    }

    FabButton {
        id: previousButton
        Layout.alignment: Qt.AlignVCenter
        backgroundImplicitSize: root.buttonSize
        padding: Style.buttonInset
        flat: true
        icon.source: MaterialIcons.skipPrevious
        onClicked: root.playPreviousFile()
    }

    FabButton {
        id: playButton
        Layout.alignment: Qt.AlignVCenter
        backgroundImplicitSize: root.buttonSize
        padding: Style.buttonInset
        flat: true
        visible: root.mediaPlayerState!==MediaPlayer.PlayingState
        icon.source: MaterialIcons.play
        onClicked: root.mediaPlayer.play()
    }

    FabButton {
        id: pausedButton
        Layout.alignment: Qt.AlignVCenter
        backgroundImplicitSize: root.buttonSize
        padding: Style.buttonInset
        flat: true
        visible: root.mediaPlayerState===MediaPlayer.PlayingState
        icon.source: MaterialIcons.pause
        onClicked: root.mediaPlayer.pause()
    }

    FabButton {
        id: nextButton
        Layout.alignment: Qt.AlignVCenter
        backgroundImplicitSize: root.buttonSize
        padding: Style.buttonInset
        flat: true
        icon.source: MaterialIcons.skipNext
        onClicked: root.playNextFile()
    }

    Item {
        Layout.fillWidth: true
    }
}
