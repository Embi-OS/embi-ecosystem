import QtQuick
import Eco.Tier3.Axion

Rectangle {
    id: root

    signal clicked()

    enum Direction {
        Up,
        Down
    }

    property int arrowDirection: ButtonSort.Up
    property color colorDefault: Style.colorPrimaryDark
    property color colorSelected: Style.colorPrimaryLight
    property bool selected: false

    implicitWidth: 51
    implicitHeight: 36

    color: selected ? colorSelected : colorDefault
    opacity: enabled ? 1.0 : 0.5

    states: State {
        name: "pressed"
        when: mouseArea.pressed
        PropertyChanges { arrow.scale: 0.9 }
    }

    SvgColorImage {
        id: arrow
        anchors.centerIn: parent
        color: Style.colorWhite
        icon: (root.arrowDirection === ButtonSort.Up) ? MaterialIcons.arrowBigUp : MaterialIcons.arrowBigDown
        size: 24
    }

    TapHandler {
        id: mouseArea
        onTapped: root.clicked()
    }
}
