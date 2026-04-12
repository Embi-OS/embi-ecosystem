import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.Label {
    id: root

    topPadding: 4
    bottomPadding: 4
    leftPadding: 8
    rightPadding: 8

    topInset: 0
    bottomInset: 0
    leftInset: 1
    rightInset: 1

    required property string display
    required property int row
    required property int column

    property color foregroundColor: ColorUtils.isDarkColor(backgroundColor) ? Style.colorWhite : Style.colorBlack
    property color backgroundColor: Style.colorPrimary
    property color backgroundBorderColor: Style.colorBlack
    property double backgroundBorderWidth: 0

    text: display
    verticalAlignment: Text.AlignVCenter
    wrapMode: Text.NoWrap
    elide: Text.ElideRight
    textFormat: Text.PlainText
    font: Style.textTheme.subtitle1
    color: foregroundColor

    opacity: enabled ? 1.0 : 0.5

    background: Rectangle {
        implicitWidth: Style.tableCellWidth
        implicitHeight: Style.tableCellHeight
        color: root.backgroundColor
        border.width: root.backgroundBorderWidth
        border.color: root.backgroundBorderColor
    }
}
