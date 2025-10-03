pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.Files
import Eco.Tier3.System

RowLayout {
    id: root

    property bool alarmEnabled: false
    property bool mediaPlaying: false

    SvgColorImage {
        visible: !Filesystem.drives.isEmpty
        icon: MaterialIcons.usb
        color: Style.colorWhite
        size: 24
    }

    SvgColorImage {
        icon: root.alarmEnabled ? MaterialIcons.alarm : MaterialIcons.alarmOff
        color: Style.colorWhite
        size: 24
    }

    SvgColorImage {
        icon: root.mediaPlaying ? MaterialIcons.music : MaterialIcons.musicOff
        color: Style.colorWhite
        size: 24
    }

    SvgColorImage {
        visible: NetworkSettingsManager.currentWifiConnection?.connected ?? false
        readonly property int signalStrength: NetworkSettingsManager.currentWifiConnection?.wirelessConfig?.signalStrength ?? 0
        icon: signalStrength < 0 ? MaterialIcons.wifiStrengthOffOutline :
              signalStrength <= 10 ? MaterialIcons.wifiStrengthOutline :
              signalStrength <= 30 ? MaterialIcons.wifiStrength1 :
              signalStrength <= 55 ? MaterialIcons.wifiStrength2 :
              signalStrength <= 80 ? MaterialIcons.wifiStrength3 :
              signalStrength <= 100 ? MaterialIcons.wifiStrength4 : MaterialIcons.wifiStrengthOffOutline
        color: Style.colorWhite
        size: 24
    }

    SvgColorImage {
        visible: NetworkSettingsManager.currentWiredConnection?.connected ?? false
        icon: MaterialIcons.ethernet
        color: Style.colorWhite
        size: 24
    }
}
