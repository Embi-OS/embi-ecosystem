pragma ComponentBehavior: Bound
import QtQuick
import QtMultimedia
import Eco.Tier1.Utils
import Eco.Tier1.Models
import Eco.Tier3.Axion
import Eco.Tier3.Media
import Eco.Tier3.Files
import L02_Clock

Item {
    id: root

    Background {
        anchors.fill: parent
    }

    W0201_Homescreen {
        id: homescreen
        anchors.fill: parent
    }

    W0200_Lockscreen {
        visible: ClockDisplay.state!==ClockDisplayStates.On
        anchors.fill: parent
    }

    MouseEventNotifier {
        id: mouseEventNotifier
        enabled: ClockDisplay.state===ClockDisplayStates.On
        window: root.Window.window as Window
        onMouseEvent: ClockDisplay.unlock()
    }

    Binding {
        target: ClockMedia
        property: "mediaPlayer"
        value: mediaPlayer
    }

    MediaPlayer {
        id: mediaPlayer
        MediaItemModel.playlist.path: Filesystem.musicPath
        MediaItemModel.playlist.selectWhen: playerSettings.selected
        audioOutput: AudioOutput {
            volume: 0.2
        }
    }

    SettingsMapper {
        id: playerSettings
        selectPolicy: QVariantMapperPolicies.Delayed
        submitPolicy: QVariantMapperPolicies.Delayed
        baseName: "media_player.conf"
        onSelectedChanged: {
            mapProperty(mediaPlayer.MediaItemModel.playlist, "path", "path")
            mapProperty(mediaPlayer.MediaItemModel.playlist, "playbackMode", "playback_mode")
            mapProperty(mediaPlayer.MediaItemModel.playlist, "shuffled", "shuffled")
        }
    }

//──────────────────────────────────────────────────────────────────────
// Déclaration des composants globaux
//──────────────────────────────────────────────────────────────────────

    LoaderDialogManager {
        anchors.fill: parent
    }

    LoaderSnackbarManager {
        visible: root.visible && !isEmpty
        position: ItemPositions.Top
    }
}
