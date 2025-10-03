pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion
import Eco.Tier3.Files
import Eco.Tier3.System

RowContainer {
    id: root

    spacing: 20
    radius: 10
    font: Style.barInfoFont

    property string menuIcon: MaterialIcons.menu
    readonly property string time: DateTimeUtils.formatTime(DateTimeUtils.systemDateTime, Locale.ShortFormat)
    readonly property string date: DateTimeUtils.formatDate(DateTimeUtils.systemDateTime)

    readonly property string language: LocaleSettings.locale
    property alias rightLayout: rightLayout

    signal menuClicked()

    ClickableIcon {
        icon: root.menuIcon
        color: Style.colorWhite
        size: 32
        onClicked: root.menuClicked()
    }

    BasicLabel {
        font: Style.textTheme.title1
        text: root.time
    }

    BasicLabel {
        font: Style.textTheme.headline6
        text: root.date
    }

    LayoutSpring {}

    RowLayout {
        id: rightLayout
        FpsIndicator {
            showMinimum: false
            showMaximum: false
            showJitter: false
        }

        BasicBusyIndicator {
            visible: Filesystem.drives.processing
            size: 24
            lineWidth: 0
        }

        SvgColorImage {
            visible: !Filesystem.drives.isEmpty
            icon: MaterialIcons.usb
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

        BasicLabel {
            font: root.font
            text: root.language
        }

        Item {
            Layout.fillHeight: true
            implicitWidth: 10
        }

        BasicLabel {
            font: Style.textTheme.title1
            text: Style.label
        }

        SvgColorImage {
            icon: Style.icon
            color: Style.colorWhite
            size: 32
            aspectRatio: Qt.KeepAspectRatioByExpanding
        }
    }
}
