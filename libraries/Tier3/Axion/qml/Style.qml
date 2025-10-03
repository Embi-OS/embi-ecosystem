pragma Singleton
import QtQuick
import Eco.Tier3.Axion

QtObject {
    id: root

    readonly property real dynamicThemeDuration: 200

    property color dynamicColorBlack: Theme.colorBlack
    property color dynamicColorWhite: Theme.colorWhite
    property color dynamicColorAccent: Theme.colorAccent
    property color dynamicColorVariant: Theme.colorVariant
    property color dynamicColorOption: Theme.colorOption
    property color dynamicColorBackground: Theme.colorBackground

    Behavior on dynamicColorBlack { ColorAnimation { duration: root.dynamicThemeDuration }}
    Behavior on dynamicColorWhite { ColorAnimation { duration: root.dynamicThemeDuration }}
    Behavior on dynamicColorAccent { ColorAnimation { duration: root.dynamicThemeDuration }}
    Behavior on dynamicColorVariant { ColorAnimation { duration: root.dynamicThemeDuration }}
    Behavior on dynamicColorOption { ColorAnimation { duration: root.dynamicThemeDuration }}
    Behavior on dynamicColorBackground { ColorAnimation { duration: root.dynamicThemeDuration }}

    readonly property bool flat: Theme.flat
    readonly property bool fine: Theme.fine
    readonly property bool sharp: Theme.sharp
    readonly property bool dense: Theme.dense

    readonly property TextTheme textTheme: Theme.textTheme

    readonly property string label: Theme.label
    readonly property string icon: Theme.icon
    readonly property string logo: Theme.logo
    readonly property string splash: Theme.splash
    readonly property int backgroundAspect: Theme.backgroundAspect
    readonly property string backgroundImage: Theme.backgroundImage

    readonly property color colorBlack: dynamicColorBlack
    readonly property color colorBlackFade: ColorUtils.transparent(colorBlack, 0.4)
    readonly property color colorWhite: dynamicColorWhite
    readonly property color colorWhiteFade: ColorUtils.transparent(colorWhite, 0.4)
    readonly property color colorTransparent: "#00000000"

    readonly property color colorAccentDark: ColorUtils.blend(colorAccent, colorBlack, 0.2)
    readonly property color colorAccent: dynamicColorAccent
    readonly property color colorAccentLight: ColorUtils.blend(colorAccent, colorWhite, 0.2)
    readonly property color colorAccentFade: ColorUtils.transparent(colorAccent, 0.4)

    readonly property color colorVariantDark: ColorUtils.blend(colorVariant, colorBlack, 0.2)
    readonly property color colorVariant: dynamicColorVariant
    readonly property color colorVariantLight: ColorUtils.blend(colorVariant, colorWhite, 0.2)
    readonly property color colorVariantFade: ColorUtils.transparent(colorVariant, 0.4)

    readonly property color colorOptionDark: ColorUtils.blend(colorOption, colorBlack, 0.2)
    readonly property color colorOption: dynamicColorOption
    readonly property color colorOptionLight: ColorUtils.blend(colorOption, colorWhite, 0.2)
    readonly property color colorOptionFade: ColorUtils.transparent(colorOption, 0.4)

    readonly property color colorBackground: dynamicColorBackground
    readonly property color colorPrimaryDarkest: Theme.colorPrimaryDarkest
    readonly property color colorPrimaryDarker: Theme.colorPrimaryDarker
    readonly property color colorPrimaryDark: Theme.colorPrimaryDark
    readonly property color colorPrimary: Theme.colorPrimary
    readonly property color colorPrimaryLight: Theme.colorPrimaryLight
    readonly property color colorPrimaryLighter: Theme.colorPrimaryLighter
    readonly property color colorPrimaryLightest: Theme.colorPrimaryLightest

    readonly property color colorTrace: Theme.colorTrace
    readonly property color colorDebug: Theme.colorDebug
    readonly property color colorInfo: Theme.colorInfo
    readonly property color colorWarning: Theme.colorWarning
    readonly property color colorError: Theme.colorError
    readonly property color colorFatal: Theme.colorFatal
    readonly property color colorSuccess: Theme.colorSuccess

    readonly property color black: Theme.black
    readonly property color white: Theme.white
    readonly property color blueGrey: Theme.blueGrey
    readonly property color grey: Theme.grey
    readonly property color brown: Theme.brown
    readonly property color deepOrange: Theme.deepOrange
    readonly property color orange: Theme.orange
    readonly property color amber: Theme.amber
    readonly property color yellow: Theme.yellow
    readonly property color lime: Theme.lime
    readonly property color green: Theme.green
    readonly property color teal: Theme.teal
    readonly property color cyan: Theme.cyan
    readonly property color lightBlue: Theme.lightBlue
    readonly property color blue: Theme.blue
    readonly property color indigo: Theme.indigo
    readonly property color deepPurple: Theme.deepPurple
    readonly property color purple: Theme.purple
    readonly property color pink: Theme.pink
    readonly property color red: Theme.red
    readonly property color oxfordBlue: Theme.oxfordBlue

    readonly property string primaryFont: root.textTheme.primaryFont

    readonly property int iconSizesMini: 8
    readonly property int iconSizesTiny: 16
    readonly property int iconSizesSmall: 24
    readonly property int iconSizesMedium: 32
    readonly property int iconSizesLarge: 48
    readonly property int iconSizesHuge: 64
    readonly property int iconSizesEnormous: 128

    function isDarkColor(color: color): bool {
        const a = 1.0 - (0.299 * color.r + 0.587 * color.g + 0.114 * color.b);
        return a >= 0.3;
    }

    function roundToIconSize(x: int): int {
        // Find the size closest to icon size
        if (x <= 0)
            return 0;
        else if (x < iconSizesMini)
            return x;
        else if (x < iconSizesTiny)
            return iconSizesMini;
        else if (x < iconSizesSmall)
            return iconSizesTiny;
        else if (x < iconSizesMedium)
            return iconSizesSmall;
        else if (x < iconSizesLarge)
            return iconSizesMedium;
        else if (x < iconSizesHuge)
            return iconSizesLarge;
        else if (x < iconSizesEnormous)
            return iconSizesHuge;
        else
            return iconSizesEnormous;
    }

    // barInfo
    readonly property double barInfoHeight: root.dense ? 16 : 20
    readonly property font barInfoFont: root.dense ? root.textTheme.headline8 : root.textTheme.headline6

    // barNavigation
    readonly property color barNavigationColor: root.colorWhite
    readonly property color barNavigationBorderColor: root.colorPrimary
    readonly property color barNavigationBorderColorHighlight: root.colorAccent
    readonly property double barNavigationBorderWidth: root.fine ? 1 : 2
    readonly property double barNavigationBorderRadius: root.sharp ? 8 : 10
    readonly property double buttonNavigationSize: root.dense ? 60 : 70
    readonly property double buttonNavigationGlow: root.fine ? 1 : 2
    readonly property double buttonNavigationCircleSize: 40
    readonly property double buttonNavigationIconSize: 32

    // page
    readonly property double titleTopMarginDefault: root.dense ? 30 : 40
    readonly property double instructionsSpaceDefault: root.dense ? 5 : 10

    readonly property double mainComponentTopMargin: titleTopMarginDefault + 100
    readonly property double mainComponentBottomMargin: root.fine ? 32 : 40
    readonly property double mainComponentLeftMargin: mainComponentRightMargin + 70
    readonly property double mainComponentRightMargin: root.fine ? 32 : 40

    readonly property double contentRectangleBorderWidth: root.fine ? 2 : 4
    readonly property double contentRectangleRadius: root.sharp ? 8 : 10

    readonly property double pageGridButtonWidth: root.dense ? 240 : 460
    readonly property double pageGridButtonHeight: root.dense ? 100 : 170
    readonly property double pageGridRowSpacing: root.dense ? 20 : 50
    readonly property double pageGridColumnSpacing: root.dense ? 20 : 50
    readonly property font pageGridButtonFont: root.dense ? root.textTheme.button : root.textTheme.title1
    readonly property double pageGridIconPadding: root.dense ? 12 : 21

    // Frame
    readonly property double frameSpacing: root.dense ? 2 : 4
    readonly property double frameBorderWidth: root.fine ? 2 : 4
    readonly property double frameRadius: root.sharp ? 8 : 10

    // Container
    readonly property double containerRadius: root.sharp ? 8 : 10
    readonly property double containerBorderWidth: root.fine ? 1 : 2
    readonly property double containerSpacing: root.dense ? 4 : 8
    readonly property double containerInset: 0

    // Ripple
    readonly property color rippleColorLight: "#20FFFFFF"
    readonly property color rippleColorDark: "#40000000"
    readonly property color rippleColor: ColorUtils.isDarkColor(colorBackground) ? rippleColorLight : rippleColorDark

    // Button
    readonly property bool buttonRound: false
    readonly property double fabButtonElevation: 4
    readonly property double fabButtonPressedElevation: 2
    readonly property double buttonElevation: root.flat ? 0 : 4
    readonly property double buttonPressedElevation: root.flat ? 0 : 2
    readonly property double buttonMinWidth: root.dense ? 32 : 40
    readonly property double buttonMinHeight: root.dense ? 32 : 40
    readonly property double buttonPadding: root.dense ? 12 : 16
    readonly property double buttonIconPadding: root.dense ? 8 : 12
    readonly property double buttonSpacing: root.dense ? 6 : 8
    readonly property double buttonInset: 4
    readonly property double buttonRadius: root.sharp ? 8 : 10
    readonly property double buttonFabSize: 48

    // Dialog
    readonly property double dialogSpacing: 4
    readonly property double dialogPadding: 10
    readonly property double dialogRadius: root.sharp ? 8 : 10
    readonly property double dialogBorderWidth: root.fine ? 2 : 4
    readonly property double dialogElevation: 24
    readonly property double dialogButtonSpacing: 0
    readonly property double dialogButtonPadding: 8
    readonly property double dialogImplicitWidth: 750
    readonly property double dialogImplicitHeight: 300

    // Popup
    readonly property double popupSpacing: 0
    readonly property double popupInset: root.dense ? 2 : 4
    readonly property double popupRadius: root.sharp ? 4 : 6
    readonly property double popupBorderWidth: root.fine ? 1 : 2
    readonly property double popupElevation: 8

    // Drawer
    readonly property double drawerElevation: 8
    readonly property double drawerRadius: root.sharp ? 8 : 10
    readonly property double drawerBorderWidth: 0 //root.fine ? 1 : 2

    // Separator
    readonly property double separatorLargePadding: root.dense ? 2 : 4
    readonly property double separatorSmallPadding: root.dense ? 1 : 2
    readonly property double separatorImplicitWidth: root.dense ? 12 : 16
    readonly property double separatorSize: root.fine ? 2 : 4

    // Menu
    readonly property double menuElevation: 8
    readonly property double menuInset: root.dense ? 2 : 4
    readonly property double menuPadding: root.dense ? 4 : 8
    readonly property double menuImplicitHeight: root.dense ? 28 : 38
    readonly property double menuImplicitWidth: root.dense ? 200 : 320
    readonly property double menuRadius: root.sharp ? 4 : 6
    readonly property double menuBorderWidth: 0 //root.fine ? 1 : 2

    // MenuItem
    readonly property font menuItemFont: root.dense ? root.textTheme.headline7 : root.textTheme.headline5
    readonly property double menuItemMinWidth: root.dense ? 32 : 48
    readonly property double menuItemMinHeight: root.dense ? 32 : 48
    readonly property double menuItemPadding: root.dense ? 8 : 12
    readonly property double menuItemIconPadding: root.dense ? 8 : 12
    readonly property double menuItemSpacing: root.dense ? 4 : 6
    readonly property double menuItemIconWidth: root.dense ? 24 : 32
    readonly property double menuItemCornerRadius: root.sharp ? 2 : 4
    readonly property double menuItemShortcutImplicitWidth: root.dense ? 74 : 96
    readonly property double menuItemTopInset: root.dense ? 1 : 2
    readonly property double menuItemBottomInset: root.dense ? 1 : 2
    readonly property double menuItemLeftInset: root.dense ? 1 : 2
    readonly property double menuItemRightInset: root.dense ? 1 : 2

    // SplitView
    readonly property double splitViewSpacing: root.dense ? 6 : 8
    readonly property double splitViewSeparatorSize: root.fine ? 2 : 4
    readonly property double splitViewHandleSize: 32
    readonly property double splitViewHandleThickness: 12
    readonly property double splitViewHandleIconSize: 24

    // TabBar
    readonly property double tabBarElevation: root.flat ? 0 : 4
    readonly property double tabBarSpacing: 0
    readonly property double tabBarInset: 0
    readonly property double tabBarPadding: 0

    // TabButton
    readonly property double tabButtonMinWidth: root.dense ? 32 : 40
    readonly property double tabButtonMinHeight: root.dense ? 32 : 40
    readonly property double tabButtonInset: root.fine ? 2 : 4
    readonly property double tabButtonPadding: 10
    readonly property double tabButtonSpacing: root.dense ? 4 : 6
    readonly property double tabButtonRadius: root.sharp ? 4 : 6
    readonly property double tabButtonIconSize: 24

    // RailBar
    readonly property double railBarElevation: root.flat ? 0 : 4
    readonly property double railBarSpacing: 0
    readonly property double railBarInset: 0
    readonly property double railBarPadding: 0

    // SideButton
    readonly property double sideButtonMinWidth: root.dense ? 48 : 52
    readonly property double sideButtonMinHeight: root.dense ? 48 : 52
    readonly property double sideButtonInset: root.dense ? 1 : 2
    readonly property double sideButtonPadding: 10
    readonly property double sideButtonSpacing: root.dense ? 4 : 6
    readonly property double sideButtonRadius: root.sharp ? 4 : 6
    readonly property double sideButtonIconSize: root.dense ? 24 : 32
    readonly property double sideButtonBorderWidth: root.fine ? 1 : 2

    // ToolBar
    readonly property double toolBarElevation: root.flat ? 0 : 4
    readonly property double toolBarSpacing: root.dense ? 12 : 16
    readonly property double toolBarImplicitHeight: root.dense ? 32 : 40

    // ToolButton
    readonly property double toolButtonMinWidth: root.dense ? 32 : 40
    readonly property double toolButtonMinHeight: root.dense ? 32 : 40
    readonly property double toolButtonRadius: root.sharp ? 8 : 10
    readonly property double toolButtonPadding: root.dense ? 2 : 4
    readonly property double toolButtonSpacing: root.dense ? 4 : 6
    readonly property double toolButtonIconWidth: root.dense ? 24 : 32
    readonly property double toolButtonInset: root.dense ? 2 : 4

    // ToolTip
    readonly property double toolTipImplicitHeight: root.dense ? 24 : 32
    readonly property double toolTipRadius: root.sharp ? 4 : 6
    readonly property double toolTipOpacity: 0.9
    readonly property double toolTipPadding: root.dense ? 8 : 16
    readonly property color toolTipColor: "#F0F0F0"

    // Snackbar
    readonly property double snackbarSpacing: root.dense ? 8 : 12
    readonly property double snackbarPadding: root.dense ? 6 : 8
    readonly property double snackbarInset: 4
    readonly property double snackbarRadius: root.sharp ? 4 : 6
    readonly property double snackbarElevation: 8
    readonly property double snackbarWidth: 400
    readonly property double snackbarProgressSize: root.fine ? 2 : 4

    // Form
    readonly property bool formShowLabelRectangle: true
    readonly property double formSummaryHeight: 85
    readonly property double formCellHeight: 44
    readonly property double formInfosHeight: 30
    readonly property double formBorderWidth: root.fine ? 2 : 4
    readonly property double formBorderRadius: root.sharp ? 4 : 6
    readonly property double formInnerMargin: 8

    // Table
    readonly property double tableCellWidth: 64
    readonly property double tableCellHeight: 32
    readonly property double tableCellBorderWidth: Style.fine ? 1 : 2
    readonly property double tableCellHeightCompact: 32
    readonly property double tableCellHeightStandard: 40
    readonly property double tableCellHeightMedium: 48
    readonly property double tableCellHeightComfortable: 64
    readonly property double tableCellWidthMaximum: 360

    // Tile
    readonly property double tileMinWidth: 200
    readonly property double tileMinHeight: 200
    readonly property double tileRadius: root.sharp ? 8 : 10
    readonly property double tileElevation: root.flat ? 0 : 4
    readonly property double tileSpacing: root.dense ? 3 : 5
    readonly property double tilePadding: root.dense ? 6 : 8
    readonly property double tileInset: 4
    readonly property double tileLineWidth: root.fine ? 8 : 10

    // Radio
    readonly property double radioIndicatorImplicitWidth: root.dense ? 16 : 18
    readonly property double radioIndicatorImplicitHeight: root.dense ? 16 : 18
    readonly property double radioIndicatorBorderWidth: root.fine ? 1 : 2
    readonly property double radioButtonHorizontalPadding: root.dense ? 6 : 8
    readonly property double radioButtonVerticalPadding: root.dense ? 6 : 14
    readonly property double radioButtonSpacing: root.dense ? 8 : 12
    readonly property double radioButtonRippleWidth: root.fine ? 18 : 28

    // Check
    readonly property double checkIndicatorImplicitWidth: root.dense ? 16 : 18
    readonly property double checkIndicatorImplicitHeight: root.dense ? 16 : 18
    readonly property double checkIndicatorIconSize: root.dense ? 16 : 18
    readonly property double checkIndicatorBorderWidth: root.fine ? 1 : 2
    readonly property double checkIndicatorRadius: 4

    // Switch
    readonly property double switchIndicatorImplicitWidth: root.dense ? 16 : 18
    readonly property double switchIndicatorImplicitHeight: root.dense ? 16 : 18

    // ItemDelegate
    readonly property double itemDelegateMinWidth: root.dense ? 32 : 48
    readonly property double itemDelegateMinHeight: root.dense ? 32 : 48
    readonly property double itemDelegateRadius: root.sharp ? 4 : 6
    readonly property double itemDelegateSpacing: root.dense ? 8 : 12
    readonly property double itemDelegatePadding: root.dense ? 8 : 12
    readonly property double itemDelegateInset: 0
    readonly property double itemDelegateWidth: root.dense ? 24 : 32

    // Slider
    readonly property double sliderSize: root.fine ? 6 : 10

    // BusyIndicator
    readonly property double busyIndicatorLineWidth: root.fine ? 6 : 10
}
