pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion

PaneTableView {
    id: root

    property alias tableViewModel: tableViewModel
    property alias proxyModel: proxyModel
    property alias sortProxyModel: sortProxyModel
    property alias transposeProxyModel: transposeProxyModel
    required property AbstractItemModel sourceModel
    required property TableViewColumnModel columnModel

    property bool sortable: true
    property int sortRow: -1
    property int sortOrder: Qt.AscendingOrder

    model: transposeProxyModel
    delegate: TableViewCompareDelegate {
        compareColumns: true
    }

    rowHeightProvider: function(row) {
        if(!tableViewModel.isColumnVisible(row))
            return 0;
        return root.defaultRowHeightProvider(row)
    }

    verticalHeaderDelegate: TableViewSortHeader {
        highlighted: sortIndex===row
        sortIndex: root.sortRow
        sortOrder: root.sortOrder
        clickable: root.sortable
        onClicked: {
            if(root.sortRow===row) {
                if(root.sortOrder===Qt.AscendingOrder)
                    root.sortOrder = Qt.DescendingOrder
                else {
                    root.sortOrder = Qt.AscendingOrder
                    root.sortRow = -1
                }
            }
            else {
                root.sortOrder = Qt.AscendingOrder
                root.sortRow = row
            }
        }
    }

    SortFilterProxyModel {
        id: proxyModel
        sourceModel: root.sourceModel
    }

    TableViewModel {
        id: tableViewModel
        sourceModel: proxyModel
        columnModel: root.columnModel
    }

    ProxyModel {
        id: sortProxyModel
        delayed: true
        sourceModel: tableViewModel
        sortColumn: root.sortRow
        sortOrder: root.sortOrder
        sortRoleName: "display"
    }

    TransposeProxyModel {
        id: transposeProxyModel
        sourceModel: sortProxyModel
    }
}
