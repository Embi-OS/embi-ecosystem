import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import Eco.Tier3.Media

RowLayout {
    id: root

    required property MediaPlayer mediaPlayer
    spacing: 10

    PlaybackCoverArt {
        id: coverArt
        Layout.fillHeight: true
        mediaPlayer: root.mediaPlayer
    }

    SongInfo {
        id: songInfo
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.maximumWidth: 200
        mediaPlayer: root.mediaPlayer

        titleFont: Style.textTheme.subtitle1
        albumFont: Style.textTheme.subtitle2
        artistFont: Style.textTheme.subtitle2
    }
}
