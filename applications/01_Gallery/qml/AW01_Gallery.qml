import QtQuick
import Eco.Tier3.Axion
import L01_Gallery

BasicWindow {
    id: root

    width: 1280
    height: 800

    frontendInitDelay: 200
    canLoadApplication: imageColorsHelper.isReady
    backend: A01_GalleryManager {}
    applicationModule: "L01_Gallery"
    applicationName: "AM01_Gallery"

    ThemeAttached.flat: true
    ThemeAttached.fine: true //false
    ThemeAttached.sharp: true //false
    ThemeAttached.dense: false

    ThemeAttached.logo: "qrc:/images/logo.svg"
    ThemeAttached.backgroundAspect: ImageFillModes.PreserveAspectCrop
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
}
