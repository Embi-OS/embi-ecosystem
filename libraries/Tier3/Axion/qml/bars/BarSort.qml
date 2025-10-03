import QtQuick
import Eco.Tier1.ProxyModel
import Eco.Tier1.Models
import Eco.Tier3.Axion

ColumnContainer {
    id: root

    spacing: 10

    property alias searchRoleName: filterBar.currentValue
    property alias searchValue: searchBar.text
    property alias sortOrder: itemSorter.sortOrder

    property alias secondaryBar: loader.sourceComponent

    property alias model: proxyModel.sourceModel

    signal clearClicked()
    signal refreshClicked()

    onClearClicked: root.reset();

    function reset() {
        filterBar.currentIndex=0;
        searchBar.text="";
        itemSorter.selectedButton=SorterItem.None;
    }

    SortFilterProxyModel {
        id: proxyModel
        filterRoleName: "visible"
        filterValue: true
    }

    RowLayout {
        id: rowLayout
        spacing: root.spacing

        FormComboBox {
            id: filterBar
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
            fitLabel: true

            label: qsTr("Filtre")
            options: proxyModel
            textRole: "title"
            valueRole: "roleName"
        }

        FormTextField {
            id: searchBar
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
            fitLabel: true

            label: qsTr("Recherche")
            placeholder: qsTr("Recherche")
        }

        SorterItem {
            id: itemSorter
            Layout.alignment: Qt.AlignVCenter
        }

        TextButton {
            Layout.alignment: Qt.AlignVCenter
            text: qsTr("Clear")
            onClicked: root.clearClicked()
        }

        IconButton {
            Layout.alignment: Qt.AlignVCenter
            icon.source: MaterialIcons.refresh
            onClicked: root.refreshClicked()
        }
    }

    Loader {
        id: loader
        visible: item
        Layout.fillWidth: true
        active: true
    }
}
