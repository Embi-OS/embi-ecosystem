import QtQuick
import Eco.Tier3.Axion
import Eco.Tier3.Media
import L02_Clock

Item {
    id: root

    readonly property MediaPlayer mediaPlayer: ClockMedia.mediaPlayer as MediaPlayer

//──────────────────────────────────────────────────────────────────────
// Déclaration des composants de la page
//──────────────────────────────────────────────────────────────────────

    AudioPlayer {
        anchors.fill: parent
        anchors.margins: 20
        mediaPlayer: root.mediaPlayer
    }
}
