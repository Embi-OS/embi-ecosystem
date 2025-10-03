import QtQuick
import Eco.Tier3.Axion

IconLabelImpl {
    id: root

    implicitWidth: implicitContentWidth
    implicitHeight: implicitContentHeight

    property double padding: 0
    topPadding: padding
    bottomPadding: padding
    leftPadding: padding
    rightPadding: padding

    property int elide: Text.ElideRight
    property int wrapMode: root.labelFillWidth ? Text.Wrap : Text.NoWrap
    property int textFormat: Text.PlainText
    property int maximumLineCount: 0
    property bool roundToIconSize: true

    delayed: true
    alignment: Qt.AlignCenter
    display: IconLabelDisplays.TextBesideIcon
    spacing: 8
    mirrored: false

    opacity: enabled ? 1.0 : 0.5
    color: Style.colorWhite
    font: Style.textTheme.subtitle1

    icon.color: root.color
    icon.width: 32
    icon.height: 32
    icon.cache: true
    icon.aspectRatio: Qt.IgnoreAspectRatio
    icon.rotation: 0

    iconItem: SvgColorImage {
        visible: root.display!==IconLabelDisplays.TextOnly && icon!==""
        icon: root.icon.source
        iconWidth: root.roundToIconSize ? Style.roundToIconSize(root.icon.width) : root.icon.width
        iconHeight: root.roundToIconSize ? Style.roundToIconSize(root.icon.height) : root.icon.height
        color: root.icon.color
        cache: root.icon.cache
        aspectRatio: root.icon.aspectRatio
        rotation: root.icon.rotation
        Behavior on rotation { NumberAnimation { duration: 200; easing.type: Easing.OutQuart } }
    }

    labelItem: Text {
        visible: root.display!==IconLabelDisplays.IconOnly && text!==""
        opacity: 1.0
        text: root.text
        font: root.font
        color: root.color
        horizontalAlignment: root.alignment & Qt.AlignHorizontal_Mask
        verticalAlignment: root.alignment & Qt.AlignVertical_Mask
        elide: root.elide
        wrapMode: root.wrapMode
        textFormat: root.textFormat
        maximumLineCount: root.maximumLineCount
    }
}
