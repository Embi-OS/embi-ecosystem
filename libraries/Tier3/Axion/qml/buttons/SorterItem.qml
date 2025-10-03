import QtQuick
import Eco.Tier3.Axion

Rectangle {
    id: root

    signal clicked()

    enum Selected {
        None,
        Up,
        Down
    }

    property int sortOrder: Qt.AscendingOrder

    property int borderSize: Style.contentRectangleBorderWidth
    property int arrowSpace: Style.contentRectangleBorderWidth

    property int selectedButton: SorterItem.None

    onSelectedButtonChanged: {
        root.sortOrder = selectedButton===SorterItem.Up ? Qt.DescendingOrder :
                         selectedButton===SorterItem.None ? Qt.AscendingOrder :
                         selectedButton===SorterItem.Down ? Qt.AscendingOrder : Qt.AscendingOrder
    }

    implicitWidth: 2 * Style.formCellHeight + arrowSpace + borderSize*2
    implicitHeight: Style.formCellHeight
    color: "transparent"
    border.width: borderSize
    border.color: Style.colorPrimary

    radius: Style.formBorderRadius

    ButtonSort {
        id: upArrow
        anchors.verticalCenter: root.verticalCenter
        anchors.left: root.horizontalCenter
        anchors.leftMargin: root.arrowSpace/2
        implicitWidth: root.width/2 - root.arrowSpace/2 - root.borderSize
        implicitHeight: root.height - root.borderSize*2

        topRightRadius: root.radius-root.borderSize
        bottomRightRadius: root.radius-root.borderSize

        arrowDirection: ButtonSort.Up
        selected: root.selectedButton === SorterItem.Up

        onClicked: {
            root.clicked()
            root.selectedButton = (root.selectedButton === SorterItem.Up ? SorterItem.None : SorterItem.Up)
        }
    }

    ButtonSort {
        id: downArrow
        anchors.verticalCenter: root.verticalCenter
        anchors.right: root.horizontalCenter
        anchors.rightMargin: root.arrowSpace/2
        implicitWidth: root.width/2 - root.arrowSpace/2 - root.borderSize
        implicitHeight: root.height - root.borderSize*2

        topLeftRadius: root.radius-root.borderSize
        bottomLeftRadius: root.radius-root.borderSize

        arrowDirection: ButtonSort.Down
        selected: root.selectedButton === SorterItem.Down

        onClicked: {
            root.clicked()
            root.selectedButton = (root.selectedButton === SorterItem.Down ? SorterItem.None : SorterItem.Down)
        }
    }
}
