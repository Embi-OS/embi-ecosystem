import QtQuick
import Eco.Tier2.Solid
import Eco.Tier3.Axion
import L00_Launcher

BasicWindow {
    id: window

    width: 1024
    height: 600

    backendInitDelay: 0
    frontendInitDelay: 500
    backend: A00_LauncherManager{}
    applicationModule: "L00_Launcher"
    applicationName: "AM00_Launcher"

    hideCursor: DisplaySettings.hideCursor
    hideKeyboard: DisplaySettings.hideKeyboard
    contentRotation: DisplaySettings.rotation

    ThemeAttached.flat: true
    ThemeAttached.fine: true
    ThemeAttached.sharp: true
    ThemeAttached.dense: false

    ThemeAttached.label: "Embi-OS"
    ThemeAttached.icon: "qrc:/images/logo.svg"
    ThemeAttached.logo: "qrc:/images/logo.svg"
    ThemeAttached.splash: "qrc:/images/logo.svg"
    ThemeAttached.backgroundImage: "qrc:/images/wallpaper/gradient.png"
    ThemeAttached.textTheme.primaryFont: Style.fine ? "Roboto" : "Frutiger LT Pro"

    ThemeAttached.colorBlack: imageColorsHelper.closestToBlack
    ThemeAttached.colorWhite: imageColorsHelper.closestToWhite
    ThemeAttached.colorAccent: imageColorsHelper.dominant
    ThemeAttached.colorVariant: imageColorsHelper.dominantComplementary
    ThemeAttached.colorBackground: imageColorsHelper.closestToBlack

    ThemeAttached.colorPrimaryDarkest: ColorUtils.blend(Style.colorPrimaryDarker, Style.black, 0.2)
    ThemeAttached.colorPrimaryDarker: ColorUtils.blend(Style.colorPrimaryDark, Style.black, 0.2)
    ThemeAttached.colorPrimaryDark: ColorUtils.blend(Style.colorPrimary, Style.black, 0.2)
    ThemeAttached.colorPrimary: ColorUtils.blend(Style.colorBackground, Style.white, 0.2)
    ThemeAttached.colorPrimaryLight: ColorUtils.blend(Style.colorPrimary, Style.white, 0.2)
    ThemeAttached.colorPrimaryLighter: ColorUtils.blend(Style.colorPrimaryLight, Style.white, 0.2)
    ThemeAttached.colorPrimaryLightest: ColorUtils.blend(Style.colorPrimaryLighter, Style.white, 0.2)

    canLoadApplication: imageColorsHelper.isReady
    ImageColorsHelper {
        id: imageColorsHelper
        source: Style.backgroundImage
        fallbackDominant: "#BC1142"
        fallbackDominantComplementary: "#00B1FF"
    }

//──────────────────────────────────────────────────────────────────────
// Déclaration du clavier virtuel
//──────────────────────────────────────────────────────────────────────

    BasicKeyboard {
        hide: window.hideKeyboard
    }
}
