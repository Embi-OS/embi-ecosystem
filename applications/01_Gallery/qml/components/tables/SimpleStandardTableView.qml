import QtQuick
import Eco.Tier3.Axion
import Qt.labs.qmlmodels

Item {
    id: root

    implicitWidth: Math.min(400, parent.width)
    implicitHeight: Math.min(300, parent.height)

    TableModel {
        id: tableModel
        TableModelColumn { display: "name" }
        TableModelColumn { display: "color" }
        TableModelColumn { display: "feet" }
        TableModelColumn { display: "Fffayatoo" }
        rows: [
            {
                "name": "cat",
                "color": "black",
                "feet": "four",
                "Fffayatoo": "no"
            },
            {
                "name": "dog",
                "color": "brown",
                "feet": "four",
                "Fffayatoo": "no"
            },
            {
                "name": "bird",
                "color": "white",
                "feet": "two",
                "Fffayatoo": "yes"
            },
            {
                "name": "bird",
                "color": "white",
                "feet": "two",
                "Fffayatoo": "yes"
            },
            {
                "name": "bird",
                "color": "white",
                "feet": "two",
                "Fffayatoo": "yes"
            },
            {
                "name": "bird",
                "color": "white",
                "feet": "two",
                "Fffayatoo": "yes"
            },
            {
                "name": "bird",
                "color": "white",
                "feet": "two",
                "Fffayatoo": "yes"
            },
            {
                "name": "bird",
                "color": "white",
                "feet": "two",
                "Fffayatoo": "yes"
            },
            {
                "name": "bird",
                "color": "white",
                "feet": "two",
                "Fffayatoo": "yes"
            },
            {
                "name": "bird",
                "color": "white",
                "feet": "two",
                "Fffayatoo": "yes"
            }
        ]
    }

    HorizontalHeaderView {
        id: horizontalHeader
        anchors.left: verticalHeader.right
        anchors.top: parent.top
        syncView: tableView
        clip: true
        textRole: "display"
        //columnWidthProvider: (column) => { return -1; }
    }

    VerticalHeaderView {
        id: verticalHeader
        anchors.top: horizontalHeader.bottom
        syncView: tableView
        clip: true
    }

    TableView {
        id: tableView
        anchors.left: verticalHeader.right
        anchors.right: parent.right
        anchors.top: horizontalHeader.bottom
        anchors.bottom: parent.bottom
        columnSpacing: 1
        rowSpacing: 1
        clip: true
        columnWidthProvider: function(column) {
            var w = view.explicitColumnWidth(column)
            if (w >= 0)
                return w;
            w = view.implicitColumnWidth(column)
            return w;
        }
        model: tableModel
        delegate: Rectangle {
            id: rectangle
            readonly property double cellPadding: 8
            readonly property var index: TableView.view.index(row, column)
            required property int row
            required property int column
            required property var display

            implicitWidth: text.implicitWidth + (cellPadding * 2)
            implicitHeight: text.implicitHeight + (cellPadding * 2)
            border.color: "darkgray"
            Text {
                id: text
                text: rectangle.display
                width: parent.width
                height: parent.height
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
