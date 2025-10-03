import QtQuick
import Eco.Tier3.Axion

Rectangle {
    id: root

    signal clicked()

    property string title: ""
    property string infos: ""
    property string comment: ""

    property bool warning: false

    property color labelColor: Style.colorBlack

    property int textTopMargin: 4
    property int textBottomMargin: 5
    property int textLeftMargin: 5
    property int textRightMargin: 5

    property bool selected: false
    property bool active: false
    property color borderColor: selected ? Style.colorAccent :
                                warning ? Style.colorWarning : color
    property alias clickable: tapHandler.enabled

    property color defaultColor: Style.grey

    implicitWidth: (ListView.view as BasicListView).viewWidth
    implicitHeight: 55

    color: defaultColor
    border.width: 2
    border.color: borderColor

    Text {
        id: titleLabel
        anchors.top: parent.top
        anchors.topMargin: root.textTopMargin
        anchors.left: parent.left
        anchors.leftMargin: root.textLeftMargin
        anchors.right: parent.right
        anchors.rightMargin: root.textRightMargin

        text: root.title
        font: Style.textTheme.title2
        color: root.labelColor
        maximumLineCount: 1
    }

    Text {
        id: infoLabel
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root.textBottomMargin
        anchors.left: parent.left
        anchors.leftMargin: root.textLeftMargin
        anchors.right: commentLabel.left
        anchors.rightMargin: root.textRightMargin

        text: root.infos
        font: Style.textTheme.body2
        color: root.labelColor
        maximumLineCount: 1
    }

    Text {
        id: commentLabel
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root.textBottomMargin
        anchors.right: parent.right
        anchors.rightMargin: root.textRightMargin

        text: root.comment
        font: Style.textTheme.body2
        color: root.labelColor
        horizontalAlignment: Qt.AlignRight
    }

    TapHandler {
        id: tapHandler
        enabled: false
        gesturePolicy: TapHandler.ReleaseWithinBounds
        onTapped: root.clicked()
    }

    HoverHandler {
        id: hoverHandler
        enabled: tapHandler.enabled
        cursorShape: Qt.PointingHandCursor
    }

    BasicVeil {
        z: 100
        anchors.fill: parent
        radius: root.radius

        relativeBackgroundColor: root.color
        focussed: tapHandler.pressed || hoverHandler.hovered
        pressed: tapHandler.pressed
    }
}
