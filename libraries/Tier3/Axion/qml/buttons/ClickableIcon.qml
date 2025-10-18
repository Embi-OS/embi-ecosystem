import QtQuick
import Eco.Tier3.Axion

SvgColorImage {
    id: root

    property bool highlighted: false
    property alias pressed: tapHandler.pressed
    property alias hovered: hoverHandler.hovered
    property alias tapCount: tapHandler.tapCount
    property alias timeHeld: tapHandler.timeHeld
    property alias gesturePolicy: tapHandler.gesturePolicy
    property alias pressedStateChanges: pressedState.changes

    property color relativeBackgroundColor: Style.colorBackground

    signal clicked()
    signal doubleClicked()
    signal longPressed()

    size: 24
    color: highlighted ? Style.colorAccent : ColorUtils.isDarkColor(relativeBackgroundColor) ? Style.colorWhite : Style.colorBlack

    states: [
        State {
            id: pressedState
            name: "pressed"
            when: tapHandler.pressed
            PropertyChanges {root.scale: 0.9 }
        }
    ]

    BasicVeil {
        z: 100
        anchors.centerIn: parent
        width: root.size*1.4
        height: width
        radius: height/2

        relativeBackgroundColor: root.relativeBackgroundColor
        focussed: root.pressed || root.hovered
        pressed: root.pressed
    }

    HoverHandler {
        id: hoverHandler
        margin: tapHandler.margin
    }

    TapHandler {
        id: tapHandler
        margin: 5
        grabPermissions: PointerHandler.TakeOverForbidden
        gesturePolicy: TapHandler.ReleaseWithinBounds
        onTapped: root.clicked()
        onDoubleTapped: root.doubleClicked()
        onLongPressed: root.longPressed()
    }
}
