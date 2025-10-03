import QtQuick
import Eco.Tier3.Axion

Item {
    id: root

    property double contentTopMargin: Style.contentRectangleRadius+Style.contentRectangleBorderWidth
    property double contentLeftMargin: Style.contentRectangleRadius+Style.contentRectangleBorderWidth
    property double contentRightMargin: Style.contentRectangleRadius+Style.contentRectangleBorderWidth
    property double contentBottomMargin: Style.contentRectangleRadius+Style.contentRectangleBorderWidth

    property int edge: 0
    readonly property bool isOnTheRight: edge & Qt.RightEdge
    readonly property bool isOnTheLeft: edge & Qt.LeftEdge
    readonly property bool isOnTheTop: edge & Qt.TopEdge
    readonly property bool isOnTheBottom: edge & Qt.BottomEdge

    property alias color: background.color
    property alias radius: background.radius
    property color borderColor: Style.colorPrimary
    property double borderWidth: Style.contentRectangleBorderWidth

    property alias background: background
    Rectangle {
        id: background
        anchors.fill: parent
        anchors.topMargin: root.isOnTheTop ? -radius: 0
        anchors.bottomMargin: root.isOnTheBottom ? -radius: 0
        anchors.leftMargin: root.isOnTheLeft ? -radius: 0
        anchors.rightMargin: root.isOnTheRight ? -radius: 0

        color: Style.colorTransparent
        radius: Style.contentRectangleRadius
        border.color: root.borderColor
        border.width: root.borderWidth
    }

    property alias haveATitle: titleCaption.haveAText
    property alias haveAComment: titleCaption.haveACaption

    property alias title: titleCaption.text
    property alias comment: titleCaption.caption
    property alias commentSpacing: titleCaption.spacing

    property double titleTopMargin: Style.contentRectangleRadius+Style.contentRectangleBorderWidth
    property double titleLeftMargin: Style.contentRectangleRadius+Style.contentRectangleBorderWidth
    property double titleRightMargin: Style.contentRectangleRadius+Style.contentRectangleBorderWidth

    property double headerHeight: (haveATitle || haveAComment) ? (Style.contentRectangleBorderWidth*2+titleTopMargin+titleCaption.height) : contentTopMargin

    property alias titleCaption: titleCaption
    LabelWithCaption {
        id: titleCaption
        anchors.top: parent.top
        anchors.topMargin: root.titleTopMargin
        anchors.left: parent.left
        anchors.leftMargin: root.titleLeftMargin
        anchors.right: parent.right
        anchors.rightMargin: root.titleRightMargin

        haveAText: !(text === "") || haveACaption
        textFont: Style.textTheme.title1
        captionFont: Style.textTheme.subtitle1
    }
}
