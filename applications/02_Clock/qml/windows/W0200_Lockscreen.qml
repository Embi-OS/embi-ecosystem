import QtQuick
import QtMultimedia
import Eco.Tier3.Axion
import Eco.Tier3.Media
import L02_Clock

Rectangle {
    id: root

    property bool nightMode: ClockDisplay.state!==ClockDisplayStates.Waking
    property bool alwaysOn: ClockDisplay.state===ClockDisplayStates.AlwaysOn

    color: Style.black
    gradient: root.alwaysOn ? null : grad

    Gradient {
        id: grad
        GradientStop { position: 0.0; color: root.nightMode ? Style.colorPrimaryDarkest : Style.colorPrimaryLight }
        GradientStop { position: 1.0; color: root.nightMode ? Style.colorPrimaryDark : Style.colorPrimaryLightest }
    }

    Background {
        anchors.fill: parent
        visible: ClockDisplay.state!==ClockDisplayStates.Off && !root.alwaysOn
    }

    SvgColorImage {
        visible: !root.alwaysOn
        anchors.centerIn: parent
        icon: root.nightMode ? MaterialIcons.weatherNight : MaterialIcons.whiteBalanceSunny
        size: 256
        color: root.nightMode ? Style.colorPrimary : ColorUtils.blend(Style.colorPrimaryLightest, Style.colorWarning, 0.4)
    }

    NumericClock {
        id: clock
        anchors.margins: 10
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        size: 64
        fontWeight: Font.Normal
        font: "Roboto"

        relativeBackgroundColor: root.color
        hour: DateTimeUtils.hour
        minute: DateTimeUtils.minute
        second: DateTimeUtils.second
        secondeVisible: false
    }

    BasicLabel {
        id: date
        anchors.left: clock.left
        anchors.bottom: clock.top
        font: Style.textTheme.headline7
        text: DateTimeUtils.formatDate(DateTimeUtils.systemDateTime)
        relativeBackgroundColor: root.color
    }

    BarInfoIcons {
        anchors.left: clock.left
        anchors.bottom: date.top
        alarmEnabled: Clock.alarmModel.enabled
        mediaPlaying: ClockMedia.mediaPlayer.playing
    }

    MiniPlayer {
        anchors.margins: 20
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        mediaPlayer: ClockMedia.mediaPlayer as MediaPlayer
    }

    TapHandler {
        onTapped: {
            if(ClockDisplay.state===ClockDisplayStates.Off)
                ClockDisplay.gotoLocked()
            else // if(ClockDisplay.state===ClockDisplayStates.Locked)
                ClockDisplay.unlock()
        }
    }
}
