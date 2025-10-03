import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

// TODO: Qt 6.10 HeaderViewDelegate
T.Control {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    verticalPadding: 4
    horizontalPadding: 8
    spacing: 5

    property double inset: 1
    topInset: inset
    bottomInset: inset
    leftInset: inset
    rightInset: inset

    required property var model
    required property string display
    required property int row
    required property int column

    property color foregroundColor: ColorUtils.isDarkColor(backgroundColor) ? Style.colorWhite : Style.colorBlack
    property color backgroundColor: highlighted ? Style.colorAccent : Style.colorPrimary

    property double backgroundImplicitWidth: Style.tableCellWidth
    property double backgroundImplicitHeight: Style.tableCellHeight
    property double backgroundBorderWidth: 0 //1
    property color backgroundBorderColor: Style.colorBlack

    font: Style.textTheme.subtitle1
    opacity: enabled ? 1.0 : 0.5

    property bool highlighted: false
    property string text: display
    property int wrapMode: Text.NoWrap
    property int elide: Text.ElideRight
    property int textFormat: Text.PlainText

    property IconObject icon: IconObject {
        width: 24
        height: 24
        color: root.foregroundColor
        source: ""
    }

    property bool clickable: true

    signal canceled()
    signal clicked()
    signal doubleClicked()
    signal pressAndHold()

    TapHandler {
        parent: root.contentItem
        enabled: root.clickable

        onCanceled: root.canceled()
        onDoubleTapped: root.doubleClicked()
        onLongPressed: root.pressAndHold()
        onTapped: root.clicked()
    }

    contentItem: IconLabel {
        opacity: 1.0
        spacing: root.spacing

        icon.source: root.icon.source
        icon.width: root.icon.width
        icon.height: root.icon.height
        icon.color: root.icon.color
        icon.cache: root.icon.cache

        alignment: Qt.AlignLeft | Qt.AlignVCenter
        font: root.font
        text: root.text
        color: root.foregroundColor
        wrapMode: root.wrapMode
        elide: root.elide
        textFormat: root.textFormat
    }

    background: Rectangle {
        implicitWidth: root.backgroundImplicitWidth
        implicitHeight: root.backgroundImplicitHeight
        color: root.backgroundColor
        border.width: root.backgroundBorderWidth
        border.color: root.backgroundBorderColor
    }
}
