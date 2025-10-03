import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.Page {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                   Math.max(contentWidth + leftPadding + rightPadding,
                   Math.max(implicitHeaderWidth, implicitFooterWidth)))
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding
                             + (implicitHeaderHeight > 0 ? implicitHeaderHeight + spacing : 0)
                             + (implicitFooterHeight > 0 ? implicitFooterHeight + spacing : 0))

    readonly property double headerHeight: root.header && root.header.visible ? root.header.height : 0
    readonly property double footerHeight: root.footer && root.footer.visible ? root.footer.height : 0

    property bool isCompleted: false
    Component.onCompleted: isCompleted=true

    property bool editable: true
    property bool navigationLocked: false
    property bool navigationEnabled: true

    property double inset: 0
    topInset: inset
    bottomInset: inset
    leftInset: inset
    rightInset: inset

    padding: Math.max(radius, borderWidth) + inset
    spacing: Style.frameSpacing

    opacity: enabled ? 1.0 : 0.5
    Behavior on opacity { NumberAnimation { duration: 150 } }

    title: ""
    property string comment: ""

    property double headerPadding: Math.max(radius, borderWidth) + inset
    property double footerPadding: Math.max(radius, borderWidth) + inset

    header: LabelWithCaption {
        topPadding: root.headerPadding
        leftPadding: root.headerPadding
        rightPadding: root.headerPadding

        visible: haveAText || haveACaption

        text: root.title
        textFont: Style.textTheme.title1

        caption: root.comment
        captionFont: Style.textTheme.subtitle1
    }

    property int edge: 0
    readonly property bool isOnTheRight: edge & Qt.RightEdge
    readonly property bool isOnTheLeft: edge & Qt.LeftEdge
    readonly property bool isOnTheTop: edge & Qt.TopEdge
    readonly property bool isOnTheBottom: edge & Qt.BottomEdge

    property bool drawFrame: false
    property bool drawHeaderSeparator: false
    property bool drawFooterSeparator: false

    property double elevation: 12
    property color color: Style.colorTransparent
    property color contentColor: Style.colorTransparent
    property color borderColor: Style.colorPrimary
    property color separatorColor: root.color===Style.colorTransparent ? Style.colorPrimary : Style.colorPrimaryLight
    property double borderWidth: drawFrame ? Style.frameBorderWidth : 0
    property double radius: drawFrame ? Style.frameRadius : 0

    background: Item {
        Rectangle {
            visible: root.drawFrame
            anchors.fill: parent
            anchors.topMargin: root.isOnTheTop ? -radius: 0
            anchors.bottomMargin: root.isOnTheBottom ? -radius: 0
            anchors.leftMargin: root.isOnTheLeft ? -radius: 0
            anchors.rightMargin: root.isOnTheRight ? -radius: 0

            radius: root.radius
            color: root.color
            border.width: root.borderWidth
            border.color: root.borderColor
        }

        Rectangle {
            visible: root.drawFrame
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
}
