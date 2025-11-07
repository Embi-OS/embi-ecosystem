import QtQuick
import Eco.Tier1.Utils
import Eco.Tier2.Solid
import Eco.Tier3.Axion
import Eco.Tier3.System
import L02_Clock

BasicWindow {
    id: root

    width: 1024
    height: 600

    backendInitDelay: 0
    frontendInitDelay: 500
    canLoadApplication: imageColorsHelper.isReady
    backend: A02_ClockManager {}
    applicationModule: "L02_Clock"
    applicationName: "AM02_Clock"

    ThemeAttached.flat: true
    ThemeAttached.fine: true
    ThemeAttached.sharp: true
    ThemeAttached.dense: false

    ThemeAttached.logo: "qrc:/images/logo.svg"
    ThemeAttached.backgroundImage: "qrc:/images/wallpaper/gradient.png"
    // ThemeAttached.backgroundImage: "qrc:/images/wallpaper/fallingIntoWater.png"
    // ThemeAttached.backgroundImage: "qrc:/images/wallpaper/neonSunset.png"
    // ThemeAttached.backgroundImage: "qrc:/images/wallpaper/wavy.png"
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

    hideCursor: DisplaySettings.hideCursor
    hideKeyboard: DisplaySettings.hideKeyboard
    contentRotation: DisplaySettings.rotation

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
        hide: root.hideKeyboard
    }

    Watchdog {
        enabled: root.backendReady // && root.frontendReady
    }

    onApplicationLoaded: Swupdate.init();
}
