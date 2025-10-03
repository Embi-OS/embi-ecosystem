import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

// TODO: Qt 6.9 TableViewDelegate
T.Control {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    verticalPadding: 4
    horizontalPadding: 8
    spacing: 5

    property double inset: 0
    topInset: inset
    bottomInset: inset
    leftInset: inset
    rightInset: inset

    required property var model
    required property int row
    required property int column
    required property bool selected
    required property bool current

    property string text
    property color foregroundColor: ColorUtils.isDarkColor(backgroundColor) ? Style.colorWhite : Style.colorBlack
    property color backgroundColor: current ? Style.colorAccent :
                                    selected ? Style.colorVariant :
                                    row % 2 ? Style.colorPrimaryDark : Style.colorPrimaryLight

    property double backgroundImplicitWidth: Style.tableCellWidth
    property double backgroundImplicitHeight: Style.tableCellHeight
    property double backgroundBorderWidth: Style.tableCellBorderWidth
    property color backgroundBorderColor: backgroundColor

    property int wrapMode: Text.Wrap
    property int elide: Text.ElideRight

    font: Style.textTheme.headline7

    opacity: enabled ? 1.0 : 0.5

    contentItem: Text {
        verticalAlignment: Text.AlignVCenter
        font: root.font
        text: root.text
        color: root.foregroundColor
        wrapMode: root.wrapMode
        elide: root.elide
    }

    background: Rectangle {
        implicitWidth: root.backgroundImplicitWidth
        implicitHeight: root.backgroundImplicitHeight
        color: root.backgroundColor
        border.width: root.backgroundBorderWidth
        border.color: root.backgroundBorderColor
    }
}
