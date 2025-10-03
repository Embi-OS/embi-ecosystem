import QtQuick
import Eco.Tier1.Utils
import Eco.Tier3.Axion

Window {
    id: root

    visible: true
    width: 1280
    height: 800
    title: Version.productName
    color: Style.colorBackground

    Material.roundedScale: Material.SmallScale
    Material.theme: Material.Dark
    Material.primary: Style.colorPrimary
    Material.accent: Style.colorAccent
    Material.background: Style.colorPrimaryDark
    Material.foreground: Style.colorWhite

    property bool hideCursor: false
    property bool hideKeyboard: false
    property double contentRotation: 0
    property string windowIcon: Style.icon

    property alias applicationHelper: applicationHelper
    ApplicationHelper {
        id: applicationHelper
        SvgIconHelper on windowIcon {
            iconWidth: 128
            iconHeight: 128
            icon: root.windowIcon
            color: Style.colorAccent
            aspectRatio: Qt.KeepAspectRatio
        }

        cursorShape: root.hideCursor ? Qt.BlankCursor : -1
        font: Style.textTheme.primaryFont
    }

    property alias backendInitDelay: splashScreenLoader.delay
    property alias frontendInitDelay: appLoader.delay
    property bool canLoadApplication: true
    required property ManagerContainer backend
    readonly property Item frontend: appLoader.item as Item

    property alias asynchronous: appLoader.asynchronous
    property alias application: appLoader.sourceComponent
    property alias applicationModule: appLoader.sourceModule
    property alias applicationName: appLoader.sourceName
    property alias splashScreen: splashScreenLoader.sourceComponent

    readonly property double contentWidth: contentRotation===0 ? root.width :
                                         contentRotation===90 ? root.height :
                                         contentRotation===180 ? root.width :
                                         contentRotation===270 ? root.height : root.width

    readonly property double contentHeight: contentRotation===0 ? root.height :
                                          contentRotation===90 ? root.width :
                                          contentRotation===180 ? root.height :
                                          contentRotation===270 ? root.width : root.height

    readonly property double keyboardWidth: contentWidth

    readonly property double keyboardHeight: !InputMethod ? 0 :
                                           contentRotation===0 ? InputMethod.keyboardRectangle.height :
                                           contentRotation===90 ? InputMethod.keyboardRectangle.width :
                                           contentRotation===180 ? InputMethod.keyboardRectangle.height :
                                           contentRotation===270 ? InputMethod.keyboardRectangle.width : InputMethod.keyboardRectangle.height

    signal applicationLoaded()

    contentItem.rotation: contentRotation
    Overlay.overlay.anchors.fill: overlayPosition
    Item {
        id: overlayPosition
        width: root.contentWidth
        height: root.contentHeight
        anchors.centerIn: parent
    }

//──────────────────────────────────────────────────────────────────────
// Loader Splashscreen
//──────────────────────────────────────────────────────────────────────

    Loadable {
        id: splashScreenLoader
        anchors.centerIn: parent
        width: root.contentWidth
        height: root.contentHeight

        delay: 0
        asynchronous: false
        sourceModule: "Eco.Tier3.Axion"
        sourceName: "Splashscreen"
        onLoaded: {
            DeviceInfo.dumpInfos();
            root.backend.init();
            root.backend.postInit();
        }
    }

//──────────────────────────────────────────────────────────────────────
// Loader Application
//──────────────────────────────────────────────────────────────────────

    Loadable {
        id: appLoader
        anchors.centerIn: parent
        width: root.contentWidth
        height: root.contentHeight

        active: root.backend.ready && root.canLoadApplication

        delay: 0
        asynchronous: true
        onLoaded: {
            root.applicationLoaded();
            splashScreenLoader.active = false;

            // Prevent app to be destroyed
            appLoader.active = true
        }
    }
}
