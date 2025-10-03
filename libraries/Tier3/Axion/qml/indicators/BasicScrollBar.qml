import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.ScrollBar {
    id: root

    property double thickness: 20
    property double radius: 4

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    policy: T.ScrollBar.AlwaysOn
    visible: policy !== T.ScrollBar.AlwaysOff
    minimumSize: orientation === Qt.Horizontal ? height / width : width / height

    //leftInset: radius/2
    //rightInset: radius/2
    padding: 0 //radius/2
    topPadding: orientation === Qt.Horizontal ? 0 : thickness
    bottomPadding: orientation === Qt.Horizontal ? 0 : thickness
    leftPadding: orientation === Qt.Vertical ? 0 : thickness
    rightPadding: orientation === Qt.Vertical ? 0 : thickness

    interactive: true

    contentItem: Rectangle {
        implicitWidth: root.interactive ? root.thickness : root.thickness/2
        implicitHeight: root.interactive ? root.thickness : root.thickness/2

        radius: root.radius
        color: Style.colorPrimary
        border.color: Style.colorPrimaryDark
        border.width: 1

        BasicVeil {
            z: 100
            anchors.fill: parent
            radius: root.radius

            relativeBackgroundColor: Style.colorPrimary
            focussed: root.pressed || root.hovered
            pressed: root.pressed
        }
    }

    background: Item {
        implicitWidth: root.interactive ? root.thickness : root.thickness/2
        implicitHeight: root.interactive ? root.thickness : root.thickness/2

        Rectangle {
            id: incrementControl
            anchors.top: root.orientation===Qt.Vertical ? parent.top : undefined
            anchors.left: root.orientation===Qt.Horizontal ? parent.left : undefined

            width: root.interactive ? root.thickness : root.thickness/2
            height: root.interactive ? root.thickness : root.thickness/2
            radius: root.radius
            color: Style.colorPrimary
            border.color: Style.colorPrimaryDark
            border.width: 1

            SvgColorImage {
                size: parent.width
                anchors.centerIn: parent
                icon: root.orientation===Qt.Horizontal ? MaterialIcons.chevronLeft : MaterialIcons.chevronUp
                color: Style.colorWhite
            }
        }

        Rectangle {
            anchors.top: root.orientation===Qt.Vertical ? incrementControl.bottom : undefined
            anchors.bottom: root.orientation===Qt.Vertical ? decrementControl.top : undefined
            anchors.left: root.orientation===Qt.Horizontal ? incrementControl.right : undefined
            anchors.right: root.orientation===Qt.Horizontal ? decrementControl.left : undefined

            implicitWidth: root.interactive ? root.thickness : root.thickness/2
            implicitHeight: root.interactive ? root.thickness : root.thickness/2

            radius: root.radius
            color: Style.colorPrimaryDarker
        }

        Rectangle {
            id: decrementControl
            anchors.bottom: root.orientation===Qt.Vertical ? parent.bottom : undefined
            anchors.right: root.orientation===Qt.Horizontal ? parent.right : undefined
            width: root.interactive ? root.thickness : root.thickness/2
            height: root.interactive ? root.thickness : root.thickness/2
            radius: root.radius
            color: Style.colorPrimary
            border.color: Style.colorPrimaryDark
            border.width: 1

            SvgColorImage {
                size: parent.width
                anchors.centerIn: parent
                icon: root.orientation===Qt.Horizontal ? MaterialIcons.chevronRight : MaterialIcons.chevronDown
                color: Style.colorWhite
            }
        }
    }

    states: State {
        name: "active"
        when: root.policy === T.ScrollBar.AlwaysOn || (root.active && root.size < 1.0)
    }

    transitions: [
        Transition {
            to: "active"
            NumberAnimation { targets: [root.contentItem, root.background]; property: "opacity"; to: 1.0 }
        },
        Transition {
            from: "active"
            SequentialAnimation {
                PropertyAction{ targets: [root.contentItem, root.background]; property: "opacity"; value: 1.0 }
                PauseAnimation { duration: 2450 }
                NumberAnimation { targets: [root.contentItem, root.background]; property: "opacity"; to: 0.0 }
            }
        }
    ]
}
