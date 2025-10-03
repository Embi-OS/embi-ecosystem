import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import Eco.Tier3.Media

Item {
    id: root

    required property MediaPlayer mediaPlayer
    readonly property MediaItemModel mediaPlaylist: mediaPlayer.MediaItemModel.playlist
    property double spacing: 10

    DialogLoader {
        active: root.mediaPlaylist.loading
        type: DialogTypes.Busy
        severity: DialogSeverities.None
        title: qsTr("Patienter")
        message: qsTr("Chargement des données en cours!")
    }

    Item {
        anchors.fill: parent
        anchors.margins: root.spacing

        RowLayout {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: seeker.top
            anchors.bottomMargin: spacing
            spacing: root.spacing*4

            PlaybackCoverArt {
                id: coverArt
                Layout.fillHeight: true
                mediaPlayer: root.mediaPlayer
            }

            ColumnLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: root.spacing

                PlaybackPlaylistControl {
                    id: playlistControl
                    Layout.fillWidth: true
                    spacing: root.spacing
                    mediaPlayer: root.mediaPlayer
                    mediaPlaylist: root.mediaPlaylist
                    isPlaylistVisible: playlistPopup.visible
                    onPlaylistClicked: playlistPopup.open()
                }

                LayoutSpring {}

                PlaybackControl {
                    id: playbackControl
                    spacing: root.spacing
                    Layout.fillWidth: true
                    mediaPlayer: root.mediaPlayer
                    onPlayNextFile: root.mediaPlaylist.next()
                    onPlayPreviousFile: root.mediaPlaylist.previous()
                }

                LayoutSpring {}

                SongInfo {
                    id: songInfo
                    Layout.fillWidth: true
                    horizontalAlignment: Qt.AlignHCenter
                    mediaPlayer: root.mediaPlayer
                }

                LayoutSpring {}

                PlaybackAudioControl {
                    id: audio
                    Layout.fillWidth: true
                    mediaPlayer: root.mediaPlayer
                }

                LayoutSpring {}
            }
        }

        PlaybackSeekControl {
            id: seeker
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            mediaPlayer: root.mediaPlayer
        }
    }

    PlaylistInfo {
        id: playlistPopup
        y: 0
        x: root.width-width
        height: root.height
        mediaPlaylist: root.mediaPlaylist
    }
}
