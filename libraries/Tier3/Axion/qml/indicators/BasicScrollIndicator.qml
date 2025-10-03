import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.ScrollIndicator {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    padding: 2

    property bool alwaysOn: false
    property color color: Style.colorPrimary

    contentItem: Rectangle {
        implicitWidth: 6
        implicitHeight: 6
        radius: 3

        color: root.color
        visible: root.size < 1.0
        opacity: root.alwaysOn ? 1.0 : 0.0

        states: State {
            name: "active"
            when: root.active && !root.alwaysOn
            PropertyChanges { root.contentItem.opacity: 0.75 }
        }

        transitions: [
            Transition {
                from: "active"
                SequentialAnimation {
                    PauseAnimation { duration: 450 }
                    NumberAnimation { target: root.contentItem; duration: 200; property: "opacity"; to: 0.0 }
                }
            }
        ]
    }
}
