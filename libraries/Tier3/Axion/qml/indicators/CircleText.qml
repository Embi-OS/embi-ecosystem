import QtQuick
import Eco.Tier3.Axion

Rectangle {
    id: root

    signal clicked()

    property color colorDefault: Style.colorWhite
    property color colorOK: Style.colorSuccess
    property color colorEnabled: Style.colorAccent
    property alias text: label.text

    property bool isOK: false
    property bool selected: false
    property bool highlighted: false

    width: 2 * radius
    height: width
    radius: 20
    color: isOK ? colorOK :
           highlighted ? colorEnabled : colorDefault
    border.color: (highlighted||isOK) ? colorDefault : colorEnabled
    border.width: selected ? Style.contentRectangleBorderWidth : 0

    BasicLabel {
        id: label
        anchors.centerIn: parent
        color: ColorUtils.isDarkColor(root.color) ? Style.colorWhite : Style.colorBlack
        font: Style.textTheme.headline5
    }

    TapHandler {
        id: mouseArea
        onTapped: root.clicked()
    }
}
