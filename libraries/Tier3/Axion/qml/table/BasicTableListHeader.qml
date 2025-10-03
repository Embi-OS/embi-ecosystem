import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion

ColumnContainer {
    id: root

    spacing: 10

    property string title: ""
    property string comment: ""

    required property PaneTableView tableView
    required property TableViewColumnModel columnModel

    property alias secondaryBar: loader.sourceComponent

    property bool refreshable: false
    signal refreshClicked()

    RowLayout {
        id: rowLayout
        spacing: root.spacing

        LabelWithCaption {
            Layout.fillWidth: true
            text: root.title
            textFont: Style.textTheme.title1

            caption: root.comment
            captionFont: Style.textTheme.subtitle1
        }

        TextButton {
            visible: root.refreshable
            text: qsTr("Refresh")
            onClicked: root.refreshClicked()
        }

        BasicSeparator {
            visible: root.refreshable
        }

        TableDensityMenuButton {
            tableView: root.tableView
        }

        TableColumnMenuButton {
            model: root.columnModel
        }
    }

    Loader {
        id: loader
        visible: item
        Layout.fillWidth: true
        active: true
    }
}
