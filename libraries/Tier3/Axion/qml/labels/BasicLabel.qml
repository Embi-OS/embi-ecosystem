import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.Label {
    id: root

    padding: 0
    topPadding: padding + topInset
    bottomPadding: padding + bottomInset
    leftPadding: padding + leftInset
    rightPadding: padding + rightInset

    font: Style.textTheme.subtitle1

    property bool highlighted: false
    property color relativeBackgroundColor: Style.colorBackground
    property color colorHighlight: Style.colorAccent

    color: {
        if(highlighted)
            return colorHighlight
        return ColorUtils.isDarkColor(relativeBackgroundColor) ? Style.colorWhite : Style.colorBlack
    }
    wrapMode: Text.NoWrap
    elide: Text.ElideRight
}
