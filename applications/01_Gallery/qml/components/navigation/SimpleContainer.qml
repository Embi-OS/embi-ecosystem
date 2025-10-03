import QtQuick
import Eco.Tier3.Axion

Item {
    id: root

    implicitWidth: Math.min(400, parent.width)
    implicitHeight: Math.min(400, parent.height)

    RowContainer {
        id: topDock
        radius: 0
        drawFrame: true
        fillHeight: true
        ExtraAnchors.topDock: root

        onCurrentIndexChanged: console.log("topDock", currentIndex)
        ButtonGroup {
            id: group
            buttons: topDock.contentChildren
            onCheckedButtonChanged: topDock.setCurrentIndex(Math.max(0, buttons.indexOf(checkedButton)))
        }

        LayoutSpring {}
        BasicSideButton {
            text: "Text 1"
        }
        BasicSideButton {
            text: "Text 2"
        }
        BasicSideButton {
            text: "Text 3"
        }
        LayoutSpring {}
    }

    RowContainer {
        id: bottomDock
        radius: 0
        drawFrame: true
        fillHeight: true
        ExtraAnchors.bottomDock: root

        onCurrentIndexChanged: console.log("bottomDock", currentIndex)
        ButtonGroup {
            buttons: bottomDock.contentChildren
            onCheckedButtonChanged: bottomDock.setCurrentIndex(Math.max(0, buttons.indexOf(checkedButton)))
        }

        LayoutSpring {}
        BasicSideButton {
            icon.source: MaterialIcons.rocket
            text: "Icon 1"
        }
        BasicSideButton {
            icon.source: MaterialIcons.car
            text: "Icon 2 long"
        }
        BasicSideButton {
            icon.source: MaterialIcons.bike
            text: "Icon 3"
        }
        LayoutSpring {}
    }

    ColumnContainer {
        id: leftDock
        radius: 0
        fillWidth: true
        drawFrame: true
        anchors.topMargin: topDock.height
        anchors.bottomMargin: bottomDock.height
        ExtraAnchors.leftDock: root

        onCurrentIndexChanged: console.log("leftDock", currentIndex)
        ButtonGroup {
            buttons: leftDock.contentChildren
            onCheckedButtonChanged: leftDock.setCurrentIndex(Math.max(0, buttons.indexOf(checkedButton)))
        }

        LayoutSpring {}
        BasicSideButton {
            icon.source: MaterialIcons.rocket
        }
        BasicSideButton {
            icon.source: MaterialIcons.car
        }
        BasicSideButton {
            icon.source: MaterialIcons.bike
        }
        BasicSideButton {
            icon.source: MaterialIcons.cog
        }
        LayoutSpring {}
    }

    ColumnContainer {
        id: rightDock
        radius: 0
        fillWidth: true
        drawFrame: true
        anchors.topMargin: topDock.height
        anchors.bottomMargin: bottomDock.height
        ExtraAnchors.rightDock: root

        onCurrentIndexChanged: console.log("rightDock", currentIndex)
        ButtonGroup {
            buttons: rightDock.contentChildren
            onCheckedButtonChanged: rightDock.setCurrentIndex(Math.max(0, buttons.indexOf(checkedButton)))
        }

        LayoutSpring {}
        BasicSideButton {
            icon.source: MaterialIcons.rocket
            text: "Footer 1"
        }
        BasicSideButton {
            icon.source: MaterialIcons.car
            text: "Footer 2"
        }
        BasicSideButton {
            icon.source: MaterialIcons.bike
            text: "Footer 3"
        }
        LayoutSpring {}
    }
}
