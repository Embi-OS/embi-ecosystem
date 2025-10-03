import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.Label {
    id: root

    font: Style.textTheme.capital

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

    padding: 4
    leftPadding: 8
    rightPadding: 8
    opacity: enabled ? 1.0 : 0.5
    color: highlighted ? Style.colorAccent : ColorUtils.isDarkColor(relativeBackgroundColor) ? Style.colorWhite : Style.colorBlack
    wrapMode: Text.NoWrap
    elide: Text.ElideRight

    states: [
        State {
            id: pressedState
            name: "pressed"
            when: tapHandler.pressed

            PropertyChanges { root.scale: 0.9 }
        }
    ]

    background: Rectangle {
        color: Style.colorTransparent
        radius: 4

        BasicVeil {
            z: 100
            anchors.fill: parent
            radius: 4

            relativeBackgroundColor: root.relativeBackgroundColor
            focussed: root.pressed || root.hovered
            pressed: root.pressed
        }

        HoverHandler {
            id: hoverHandler
        }

        TapHandler {
            id: tapHandler
            gesturePolicy: TapHandler.ReleaseWithinBounds
            onTapped: root.clicked()
            onDoubleTapped: root.doubleClicked()
            onLongPressed: root.longPressed()
        }
    }
}
