import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.ItemDelegate {
    id: root

    property bool expanded: false
    property bool hasChildren: false
    property int depth

    implicitWidth: Math.max(background ? implicitBackgroundWidth : 0,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(background ? implicitBackgroundHeight : 0,
                             implicitContentHeight + topPadding + bottomPadding)

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    spacing: Style.buttonSpacing

    icon.source: root.hasChildren ? MaterialIcons.chevronRight : ""
    icon.color: color
    icon.width: (root.font.pixelSize/3)*4
    icon.height: (root.font.pixelSize/3)*4
    property double iconRotation: root.expanded ? 90 : 0

    font: Style.textTheme.subtitle1
    property color color: root.highlighted ? Style.colorAccent : Style.colorWhite
    property int wrapMode: Text.NoWrap
    property int elide: Text.ElideRight
    property int textFormat: Text.PlainText

    contentItem: IconLabel {
        opacity: 1.0
        spacing: root.spacing

        icon.source: root.icon.source
        icon.width: root.icon.width
        icon.height: root.icon.height
        icon.color: root.icon.color
        icon.cache: root.icon.cache
        icon.rotation: root.iconRotation

        alignment: Qt.AlignLeft
        font: root.font
        text: root.text
        color: root.color
        wrapMode: root.wrapMode
        elide: root.elide
        textFormat: root.textFormat
    }

    background: Item {
        BasicVeil {
            anchors.fill: parent
            visible: root.hasChildren
            focussed: root.down || root.visualFocus || root.hovered
            pressed: root.pressed
        }
    }
}
