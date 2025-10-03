pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.Files
import Eco.Tier3.System

RowContainer {
    id: root

    spacing: 20
    radius: 10
    font: Style.barInfoFont

    signal menuClicked()

    property bool alarmEnabled: false
    property bool mediaPlaying: false

    readonly property string time: DateTimeUtils.formatTime(DateTimeUtils.systemDateTime, Locale.ShortFormat)
    readonly property string date: DateTimeUtils.formatDate(DateTimeUtils.systemDateTime)

    readonly property string language: LocaleSettings.locale
    readonly property LocaleFilterModel langModel: LocaleFilterModel {
        filters: [
            "C",
            "fr_CH",
            "de_CH",
            "it_CH",
            "en_GB"
        ]
    }

    function changeLanguage(language: string) {
        if(!LocaleSettings.setLocale(language))
            return;
        AxionHelper.warningRestart()
    }

    ClickableIcon {
        icon: MaterialIcons.menu
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

        BarInfoIcons {
            Layout.fillHeight: true
            alarmEnabled: root.alarmEnabled
            mediaPlaying: root.mediaPlaying
        }

        Item {
            Layout.fillHeight: true
            implicitWidth: 10
        }

        BasicLabel {
            font: Style.textTheme.title1
            text: "PiClock"
        }

        SvgColorImage {
            icon: Style.icon
            color: Style.colorWhite
            size: 32
        }
    }

    TapHandler {
        parent: rightLayout
        // margin: rightLayout.padding
        target: rightLayout
        cursorShape: Qt.PointingHandCursor
        onTapped: menu.open()
    }

    BasicMenu {
        id: menu
        ExtraPosition.position: ItemPositions.BottomRight
        BasicMenu {
            id: diskMenu
            title: qsTr("Disques / USB")
            onOpened: Filesystem.drives.markDirty()

            section.property: "driveDisplayType"
            section.delegate: BasicLabel {
                width: (ListView.view as ListView).width
                topInset: Style.menuItemTopInset
                bottomInset: Style.menuItemBottomInset
                leftInset: Style.menuItemLeftInset
                rightInset: Style.menuItemRightInset
                required property string section
                text: qsTr("Disque(s): %1").arg(section)
                font: Style.textTheme.headline7
            }

            Repeater {
                model: ProxyModel {
                    delayed: true
                    enabled: diskMenu.opened
                    sourceModel: Filesystem.drives
                    sortRoleName: "driveDisplayType"
                }
                delegate: BasicMenuItem {
                    required property FilesystemDrive qtObject
                    required property string name
                    required property string driveDevice
                    required property string fileUrl
                    required property string driveDisplayType
                    required property bool driveIsNetwork
                    enabled: !driveIsNetwork
                    hint: enabled ? qsTr("Ejecter") : ""
                    text: ("%1 (%2)").arg(name).arg(driveDevice)
                    icon.source: MimeIconHelper.getSvgIconPathForUrl(fileUrl)
                    onTriggered: Filesystem.drives.eject(qtObject)
                }
            }
        }
        BasicMenu {
            id: langMenu
            title: qsTr("Langue")
            width: 200

            Repeater {
                model: ProxyModel {
                    delayed: true
                    enabled: langMenu.opened
                    sourceModel: root.langModel
                }
                delegate: BasicMenuItem {
                    required property string name
                    outlined: name===root.language
                    text: name
                    onTriggered: root.changeLanguage(text)
                }
            }
        }
        BasicMenuSeparator {}
        BasicMenuItem {icon.source: MaterialIcons.informationOutline; onTriggered: AxionHelper.showAbout(); text: qsTr("A propos")}
        BasicMenuSeparator {}
        BasicMenuItem {icon.source: MaterialIcons.power; onTriggered: powerPopup.open(); text: qsTr("Eteindre / Redémarrer")}
    }

    DialogPower {
        id: powerPopup
    }
}
