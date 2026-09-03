pragma ComponentBehavior: Bound
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
        height: root.height-20
        interactive: false
    }

    contentItem: Item {
        SwipeView {
            id: swipeView
            anchors.fill: parent
            anchors.bottomMargin: pageIndicator.height
            currentIndex: 2

            ViewDelegateLoader {
                asynchronous: false
                StandardObject.icon: MaterialIcons.homeAssistant
                StandardObject.text: "Home-Assistant"
                W0201_Tab4_HassBedroom {}
            }
            ViewDelegateLoader {
                active: true
                asynchronous: false
                StandardObject.icon: MaterialIcons.alarm
                StandardObject.text: "Alarm"
                W0201_Tab1_AlarmSettings {}
            }
            ViewDelegateLoader {
                active: true
                asynchronous: false
                StandardObject.icon: MaterialIcons.clockOutline
                StandardObject.text: "Clock"
                W0201_Tab2_Clock {}
            }
            ViewDelegateLoader {
                active: true
                asynchronous: false
                StandardObject.icon: MaterialIcons.music
                StandardObject.text: "Music"
                W0201_Tab3_AudioPlayer {}
            }
        }

        BasicPageIndicator {
            id: pageIndicator
            padding: 0
            count: swipeView.count
            currentIndex: swipeView.currentIndex
            anchors.bottom: parent.bottom
            anchors.bottomMargin: height/2
            anchors.horizontalCenter: parent.horizontalCenter

            delegate: SvgColorImage {
                required property int index
                size: 16
                icon: swipeView.itemAt(index).StandardObject.icon
                color: index===pageIndicator.currentIndex  ? pageIndicator.color : Style.colorPrimaryDark
                opacity: root.enabled ? 1.0 : 0.7
            }
        }
    }
}
