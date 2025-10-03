pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.Dialog {
    id: root

    property bool animation: true

    readonly property double dialogImplicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                                                  Math.max(contentWidth + leftPadding + rightPadding,
                                                  Math.max(implicitHeaderWidth, implicitFooterWidth)))
    readonly property double dialogImplicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                                                            contentHeight + topPadding + bottomPadding
                                                            + (implicitHeaderHeight > 0 ? implicitHeaderHeight + spacing : 0)
                                                            + (implicitFooterHeight > 0 ? implicitFooterHeight + spacing : 0))

    readonly property double headerHeight: root.header && root.header.visible ? root.header.height : 0
    readonly property double footerHeight: root.footer && root.footer.visible ? root.footer.height : 0

    property bool fullscreen: false
    property double preferredWidth: Style.dialogImplicitWidth
    property double preferredHeight: Style.dialogImplicitHeight

    readonly property BasicWindow rootWindow: parent.Window.window as BasicWindow
    readonly property double windowWidth: rootWindow?.contentWidth ?? rootWindow?.width ?? 0
    readonly property double windowHeight: rootWindow?.contentHeight ?? rootWindow?.height ?? 0
    readonly property double keyboardHeight: rootWindow?.keyboardHeight ?? 0
    readonly property bool keyboardVisible: InputMethod && InputMethod.visible
    readonly property double parentWidth: windowWidth
    readonly property double parentHeight: windowHeight - (keyboardVisible ? keyboardHeight : 0)
    readonly property double maximumWidth: parentWidth - 2*padding
    readonly property double maximumHeight: parentHeight - 2*padding

    implicitWidth: fullscreen ? maximumWidth : Math.ceil(Math.min(maximumWidth, Math.max(dialogImplicitWidth,preferredWidth)))
    implicitHeight: fullscreen ? maximumHeight : Math.ceil(Math.min(maximumHeight, Math.max(dialogImplicitHeight,preferredHeight)))

    x: Math.floor((parentWidth - width) / 2) - parent.ScenePosition.x
    y: Math.floor((parentHeight - height) / 2) - parent.ScenePosition.y

    property double inset: 0
    topInset: inset
    bottomInset: inset
    leftInset: inset
    rightInset: inset

    padding: Math.max(radius, borderWidth)
    spacing: Style.dialogSpacing

    font: Style.textTheme.body1

    modal: true // Back darker
    focus: true
    closePolicy: T.Popup.NoAutoClose

    enter: Transition {
        enabled: root.animation
        NumberAnimation { property: "scale";from: 0.9;to: 1.0;easing.type: Easing.OutQuint; duration: 220 }
        NumberAnimation { property: "opacity";from: 0.0;to: 1.0;easing.type: Easing.OutCubic; duration: 150 }
    }

    exit: Transition {
        enabled: root.animation
        NumberAnimation { property: "scale";from: 1.0;to: 0.9;easing.type: Easing.OutQuint; duration: 220 }
        NumberAnimation { property: "opacity";from: 1.0;to: 0.0;easing.type: Easing.OutCubic; duration: 150 }
    }

    T.Overlay.modal: BasicOverlay {
        Behavior on opacity { enabled: root.animation; NumberAnimation { duration: 150 } }
    }
    T.Overlay.modeless: BasicOverlay {
        Behavior on opacity { enabled: root.animation; NumberAnimation { duration: 150 } }
    }

    title: ""
    property string message: ""

    property double headerPadding: Math.max(radius, borderWidth)
    property double footerPadding: Math.max(radius, borderWidth)

    property int headerHorizontalAlignment: Qt.AlignLeft
    property int headerVerticalAlignment: Qt.AlignTop

    header: LabelWithCaption {
        topPadding: root.headerPadding
        leftPadding: root.headerPadding
        rightPadding: root.headerPadding

        horizontalAlignment: root.headerHorizontalAlignment
        verticalAlignment: root.headerVerticalAlignment

        visible: haveAText || haveACaption

        text: root.title
        textFont: Style.textTheme.title1

        caption: root.message
        captionFont: Style.textTheme.subtitle1
    }

    property double elevation: Style.dialogElevation
    property color color: Style.colorPrimary
    property color contentColor: Style.colorTransparent
    property color borderColor: Style.colorPrimary
    property color separatorColor: Style.colorPrimaryLight
    property double borderWidth: Style.dialogBorderWidth
    property double radius: Style.dialogRadius

    property bool drawHeaderSeparator: false
    property bool drawFooterSeparator: false

    background: Item {
        layer.enabled: root.elevation>0
        layer.effect: BasicElevationEffect {
            elevation: root.elevation
        }

        Rectangle {
            anchors.fill: parent
            radius: root.radius
            color: root.color
            border.width: root.borderWidth
            border.color: root.borderColor
        }

        Rectangle {
            anchors.fill: parent
            radius: root.radius-root.borderWidth
            color: root.contentColor
            anchors.topMargin: root.borderWidth + (root.header && root.header.visible ? (root.header.height + root.spacing) : 0)
            anchors.bottomMargin: root.borderWidth + (root.footer && root.footer.visible ? (root.footer.height + root.spacing) : 0)
            anchors.leftMargin: root.borderWidth
            anchors.rightMargin: root.borderWidth
        }

        BasicSeparator {
            visible: root.drawHeaderSeparator && root.headerHeight>0
            orientation: Qt.Horizontal
            anchors.horizontalCenter: parent.horizontalCenter
            foregroundColor: root.separatorColor
            implicitWidth: parent.width - root.borderWidth*2
            y: root.header ? root.header.height + root.topPadding/2 - height/2 + root.spacing/2 : 0
        }

        BasicSeparator {
            visible: root.drawFooterSeparator && root.footerHeight>0
            orientation: Qt.Horizontal
            anchors.horizontalCenter: parent.horizontalCenter
            foregroundColor: root.separatorColor
            implicitWidth: parent.width - root.borderWidth*2
            y: root.footer ? root.height - root.footer.height - root.bottomPadding/2 - height/2 - root.spacing/2 : 0
        }
    }

    property alias buttonsContainer: buttonBox.contentData

    footer: BasicDialogButtonBox {
        id: buttonBox
        visible: count>0
    }
}
