import QtQuick
import Eco.Tier3.Axion

LabelWithCaptionImpl {
    id: root

    implicitWidth: implicitContentWidth
    implicitHeight: implicitContentHeight

    property double padding: 0
    topPadding: padding
    bottomPadding: padding
    leftPadding: padding
    rightPadding: padding

    delayed: true
    spacing: 0
    opacity: enabled ? 1.0 : 0.5

    horizontalAlignment: Qt.AlignLeft
    verticalAlignment: Qt.AlignTop

    haveAText: !(text === "")
    haveACaption: !(caption === "")

    property color color: ColorUtils.isDarkColor(relativeBackgroundColor) ? Style.colorWhite : Style.colorBlack
    property color relativeBackgroundColor: Style.colorBackground

    text: ""
    textFont: Style.textTheme.body1
    textColor: color
    property int textElide: Text.ElideRight
    property int textWrapMode: Text.NoWrap
    property int textMaximumLineCount: 1

    caption: ""
    captionFont: Style.textTheme.caption1
    captionColor: color
    property int captionElide: Text.ElideRight
    property int captionWrapMode: Text.Wrap
    property int captionMaximumLineCount: 1

    labelItem: Text {
        visible: root.haveAText
        text: root.text
        font: root.textFont
        color: root.textColor
        horizontalAlignment: root.horizontalAlignment & Qt.AlignHorizontal_Mask
        verticalAlignment: Qt.AlignVCenter
        elide: root.textElide
        wrapMode: root.textWrapMode
        maximumLineCount: root.textMaximumLineCount
    }

    captionItem: Text {
        visible: root.haveACaption
        text: root.caption
        font: root.captionFont
        color: root.captionColor
        horizontalAlignment: root.horizontalAlignment & Qt.AlignHorizontal_Mask
        verticalAlignment: Qt.AlignVCenter
        elide: root.captionElide
        wrapMode: root.captionWrapMode
        maximumLineCount: root.captionMaximumLineCount
    }
}
