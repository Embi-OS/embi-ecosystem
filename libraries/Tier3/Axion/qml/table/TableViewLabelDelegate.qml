import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.Label {
    id: root

    topPadding: 4
    bottomPadding: 4
    leftPadding: 8
    rightPadding: 8

    property double inset: 0
    topInset: inset
    bottomInset: inset
    leftInset: inset
    rightInset: inset

    required property int row
    required property int column
    required property bool selected
    required property bool current

    property color foregroundColor: ColorUtils.isDarkColor(backgroundColor) ? Style.colorWhite : Style.colorBlack
    property color backgroundColor: current ? Style.colorAccent :
                                    selected ? Style.colorVariant :
                                    row % 2 ? Style.colorPrimaryDark : Style.colorPrimaryLight
    property color backgroundBorderColor: backgroundColor
    property double backgroundBorderWidth: Style.tableCellBorderWidth

    verticalAlignment: Text.AlignVCenter
    wrapMode: Text.Wrap
    elide: Text.ElideRight
    textFormat: Text.PlainText
    font: Style.textTheme.headline7
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
