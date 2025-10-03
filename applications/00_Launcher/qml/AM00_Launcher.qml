pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import Eco.Tier3.System
import L00_Launcher

Page {
    id: root

    background: Background {}

    header: LauncherStatusBar {
        menuIcon: MaterialIcons.menu
        onAboutClicked: AxionHelper.showAbout()
        onPowerClicked: powerPopup.open()
        onLangChanged: AxionHelper.warningRestart()
        onMenuClicked: drawer.open()
    }

    contentItem: SettingsLauncherView {}

//──────────────────────────────────────────────────────────────────────
// Déclaration des composants globaux
//──────────────────────────────────────────────────────────────────────

    BasicDrawer {
        id: drawer
        edge: Qt.TopEdge
        width: root.width
        height: root.height-root.header.height
        interactive: false
        contentItem: PaneSplitView {
            padding: 10
            proxyModel.enabled: visible
            active: drawer.position>=1.0
            drawHeaderSeparator: true
            model: LauncherSettingsModel {}

            header: LauncherInfosBar {
                menuIcon: MaterialIcons.menu
                onMenuClicked: drawer.close()
            }
        }
    }

    DialogPower {
        id: powerPopup
        quitVisible: false
        suspendVisible: false
    }

    LoaderDialogManager {
        anchors.fill: parent
    }

    LoaderSnackbarManager {
        visible: root.visible && !isEmpty
        position: ItemPositions.Top
    }
}
