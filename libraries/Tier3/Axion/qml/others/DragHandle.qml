import QtQuick
import Eco.Tier3.Axion

Rectangle {
    id: root

    enum Direction {
        Right,
        Left,
        Top,
        Bottom
    }

    property int handleSize: 30
    property int minSize: 0
    property int maxSize: 300

    property int direction: DragHandle.Direction.Right
    property bool isDragable: true

    property bool isDoubleClicking: false

    property Item resizableItem
    parent: resizableItem

    radius: Style.contentRectangleBorderWidth
    color: Style.colorPrimary

    state: direction===DragHandle.Direction.Right ? "right" :
           direction===DragHandle.Direction.Left ? "left" :
           direction===DragHandle.Direction.Top ? "top" :
           direction===DragHandle.Direction.Bottom ? "bottom" : "right"

    states: [
        State {
            name: "right"
            PropertyChanges { root.anchors.verticalCenter: root.resizableItem.verticalCenter }
            PropertyChanges { root.anchors.left: root.resizableItem.right }
            PropertyChanges { root.anchors.leftMargin: -root.radius }
            PropertyChanges { root.width: root.handleSize }
            PropertyChanges { root.height: root.handleSize*4 }
            PropertyChanges { image.rotation: 90 }
        },
        State {
            name: "left"
            PropertyChanges { root.anchors.verticalCenter: root.resizableItem.verticalCenter }
            PropertyChanges { root.anchors.right: root.resizableItem.left }
            PropertyChanges { root.anchors.rightMargin: -root.radius }
            PropertyChanges { root.width: root.handleSize }
            PropertyChanges { root.height: root.handleSize*4 }
            PropertyChanges { image.rotation: -90 }
        },
        State {
            name: "top"
            PropertyChanges { root.anchors.horizontalCenter: root.resizableItem.horizontalCenter }
            PropertyChanges { root.anchors.bottom: root.resizableItem.top }
            PropertyChanges { root.anchors.bottomMargin: -root.radius }
            PropertyChanges { root.width: root.handleSize*4 }
            PropertyChanges { root.height: root.handleSize }
            PropertyChanges { image.rotation: 0 }
        },
        State {
            name: "bottom"
            PropertyChanges { root.anchors.horizontalCenter: root.resizableItem.horizontalCenter }
            PropertyChanges { root.anchors.top: root.resizableItem.bottom }
            PropertyChanges { root.anchors.topMargin: -root.radius }
            PropertyChanges { root.width: root.handleSize*4 }
            PropertyChanges { root.height: root.handleSize }
            PropertyChanges { image.rotation: 180 }
        }
    ]

    SvgColorImage {
        id: image
        anchors.centerIn: parent
        color: ColorUtils.isDarkColor(root.color) ? Style.colorWhite : Style.colorBlack
        icon: MaterialIcons.menuUp
        size: 0
        iconWidth: root.handleSize
        aspectRatio: Qt.KeepAspectRatioByExpanding
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onDoubleClicked: {
            root.isDoubleClicking = true;

            var newWidth = root.resizableItem.width;
            var newHeight = root.resizableItem.height;
            var newX = root.resizableItem.x;
            var newY = root.resizableItem.y;

            switch(root.direction) {
            case DragHandle.Direction.Right:
            case DragHandle.Direction.Left:
                if(root.resizableItem.width < root.maxSize*0.8)
                    newWidth = root.maxSize;
                else if(root.resizableItem.width >= root.maxSize*0.8)
                    newWidth = root.minSize;

                if(root.direction===DragHandle.Direction.Left)
                    newX = root.resizableItem.x - (newWidth-root.resizableItem.width);
                break;
            case DragHandle.Direction.Bottom:
            case DragHandle.Direction.Top:
                if(root.resizableItem.height < root.maxSize*0.8)
                    newHeight = root.maxSize;
                else if(root.resizableItem.height >= root.maxSize*0.8)
                    newHeight = root.minSize;

                if(root.direction===DragHandle.Direction.Top)
                    newY = root.resizableItem.y - (newHeight-root.resizableItem.height);
                break;
            default:
                break;
            }

            root.resizableItem.width = newWidth;
            root.resizableItem.height = newHeight;
            root.resizableItem.x = newX;
            root.resizableItem.y = newY;

            root.isDoubleClicking=false;
        }

        onPositionChanged: {
            if(root.isDoubleClicking)
                return;

            if(!root.isDragable)
                return;

            var shiftX = -mouseX
            var shiftY = -mouseY

            var newWidth = root.resizableItem.width;
            var newHeight = root.resizableItem.height;
            var newX = root.resizableItem.x;
            var newY = root.resizableItem.y;

            switch(root.direction) {
            case DragHandle.Direction.Right:
                newWidth = Math.max(root.minSize, Math.min(root.resizableItem.width - shiftX, root.maxSize));
                break;
            case DragHandle.Direction.Left:
                newWidth = Math.max(root.minSize, Math.min(root.resizableItem.width + shiftX, root.maxSize));
                newX = root.resizableItem.x - (newWidth-root.resizableItem.width);
                break;
            case DragHandle.Direction.Bottom:
                newHeight = Math.max(root.minSize, Math.min(root.resizableItem.height - shiftY, root.maxSize));
                break;
            case DragHandle.Direction.Top:
                newHeight = Math.max(root.minSize, Math.min(root.resizableItem.height + shiftY, root.maxSize));
                newY = root.resizableItem.y - (newHeight-root.resizableItem.height);
                break;
            default:
                break;
            }

            root.resizableItem.width = newWidth;
            root.resizableItem.height = newHeight;
            root.resizableItem.x = newX;
            root.resizableItem.y = newY;
        }
    }
}
