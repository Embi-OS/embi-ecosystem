import QtQuick
import Eco.Tier3.Axion

SlidingItemImpl {
    id: root

    implicitWidth: implicitContentWidth + leftPadding + rightPadding
    implicitHeight: implicitContentHeight + topPadding + bottomPadding

    property double padding: 0
    topPadding: padding
    bottomPadding: padding
    leftPadding: padding
    rightPadding: padding

    clip: true
    delayed: true
    opacity: enabled ? 1.0 : 0.5

    horizontalAlignment: Qt.AlignLeft
    verticalAlignment: Qt.AlignVCenter
    vertical: false

    property string text: ""
    property font font: Style.textTheme.subtitle1
    property int elide: Text.ElideNone
    property int wrapMode: Text.Wrap

    property bool highlighted: false
    property color relativeBackgroundColor: Style.colorBackground
    property color colorHighlight: Style.colorAccent

    property color color: {
        if(highlighted)
            return colorHighlight
        return ColorUtils.isDarkColor(relativeBackgroundColor) ? Style.colorWhite : Style.colorBlack
    }

    contentItem: Text {
        text: root.text
        font: root.font
        color: root.color
        horizontalAlignment: root.horizontalAlignment & Qt.AlignHorizontal_Mask
        verticalAlignment: root.verticalAlignment & Qt.AlignVertical_Mask
        elide: root.elide
        wrapMode: root.wrapMode
    }
}
