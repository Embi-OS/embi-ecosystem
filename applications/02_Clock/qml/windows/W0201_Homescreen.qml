import QtQuick
import QtMultimedia
import Eco.Tier1.Models
import Eco.Tier3.Axion
import L02_Clock

Page {
    id: root

    background: null

    property QtMediaPlayer test: ClockMedia.mediaPlayer

    header: TopBarInfo {
        alarmEnabled: Clock.alarmModel.enabled
        mediaPlaying: ClockMedia.mediaPlayer.playing
        onMenuClicked: drawer.open()
    }

    W0202_Drawer {
        id: drawer
        edge: Qt.TopEdge
        width: root.width
        height: root.height-root.header.height
        interactive: false
    }

    contentItem: Item {
        SwipeView {
            id: swipeView
            anchors.fill: parent

            W0201_Tab1_AlaramSettings {
                alarmModel: Clock.alarmModel
            }

            W0201_Tab2_Clock {}

            W0201_Tab3_AudioPlayer {
                mediaPlayer: ClockMedia.mediaPlayer as MediaPlayer
            }

            Component.onCompleted: setCurrentIndex(1)
        }

        BasicPageIndicator {
            count: swipeView.count
            currentIndex: swipeView.currentIndex
            anchors.bottom: swipeView.bottom
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
