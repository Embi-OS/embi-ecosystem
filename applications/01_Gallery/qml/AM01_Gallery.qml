pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

Page {
    id: root

    Binding {
        when: themeButton.checked
        delayed: true

        root.ThemeAttached.logo: "qrc:/images/homeVoh.svg"
        root.ThemeAttached.backgroundAspect: ImageFillModes.PreserveAspectFit
        root.ThemeAttached.backgroundImage: ""

        root.ThemeAttached.colorBlack: "#000000"
        root.ThemeAttached.colorWhite: "#FFFFFF"
        root.ThemeAttached.colorAccent: "#DD1940"
        root.ThemeAttached.colorVariant: "#00B1FF"
        root.ThemeAttached.colorOption: "#FF55FF"

        root.ThemeAttached.colorTrace: "#777777"
        root.ThemeAttached.colorDebug: Theme.grey
        root.ThemeAttached.colorInfo: Theme.blue
        root.ThemeAttached.colorWarning: Theme.yellow
        root.ThemeAttached.colorError: Theme.orange
        root.ThemeAttached.colorFatal: Theme.red
        root.ThemeAttached.colorSuccess: Theme.green

        root.ThemeAttached.colorBackground: "#282B2C"
        root.ThemeAttached.colorPrimaryDarkest: ColorUtils.blend(Style.colorPrimaryDarker, Style.black, 0.2)
        root.ThemeAttached.colorPrimaryDarker: ColorUtils.blend(Style.colorPrimaryDark, Style.black, 0.2)
        root.ThemeAttached.colorPrimaryDark: "#2E3132"
        root.ThemeAttached.colorPrimary: "#36393B"
        root.ThemeAttached.colorPrimaryLight: "#44474A"
        root.ThemeAttached.colorPrimaryLighter: ColorUtils.blend(Style.colorPrimaryLight, Style.white, 0.2)
        root.ThemeAttached.colorPrimaryLightest: ColorUtils.blend(Style.colorPrimaryLighter, Style.white, 0.2)
    }

    header: RowContainer {
        drawFrame: true
        radius: 0
        BasicMenuItem {
            text: "Style"
            onClicked: styleMenu.open()
            BasicMenu {
                id: styleMenu
                BasicSwitchDelegate {
                    id: themeButton
                    text: "VOH.ch"
                    // checked: true
                }
                BasicMenuSeparator {}
                BasicSwitchDelegate {
                    text: "Flat"
                    checked: Theme.flat
                    onClicked: Theme.flat=checked
                }
                BasicSwitchDelegate {
                    text: "Fine"
                    checked: Theme.fine
                    onClicked: Theme.fine=checked
                }
                BasicSwitchDelegate {
                    text: "Sharp"
                    checked: Theme.sharp
                    onClicked: Theme.sharp=checked
                }
                BasicSwitchDelegate {
                    text: "Dense"
                    checked: Theme.dense
                    onClicked: Theme.dense=checked
                }
                BasicMenuSeparator {}
                BasicMenuItem {
                    text: "Quit"
                    onClicked: Qt.quit()
                }
                BasicMenuItem {
                    text: "Exit"
                    onClicked: Qt.exit(1)
                }
            }
        }
        LayoutSpring {}
        FpsIndicator {
            Layout.alignment: Qt.AlignVCenter
        }
    }

    contentItem: Item {
        B01_Gallery {
            anchors.fill: parent
            StandardObject.icon: MaterialIcons.atom
            StandardObject.text: "Axion"
        }

        LoaderDialogManager {
            anchors.fill: parent
        }

        LoaderSnackbarManager {
            visible: root.visible && !isEmpty
            position: ItemPositions.Top
        }
    }

    background: Background {}
}
