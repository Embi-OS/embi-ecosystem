pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

PaneTableView {
    id: root

    property alias tableViewModel: tableViewModel
    required property VariantListModel sourceModel
    required property TableViewColumnModel columnModel

    readonly property bool hasCurrent: view.currentRow>=0
    property bool paginated: true
    property bool sortable: true
    property int sortColumn: -1
    property int sortOrder: Qt.AscendingOrder

    property bool filterVisible: true
    property bool dateVisible: true
    property alias fromDate: dateRange.fromDate
    property alias fromDateValue: dateRange.fromDateValue
    property alias toDate: dateRange.toDate
    property alias toDateValue: dateRange.toDateValue

    property alias dateRange: dateRange
    property alias filterMenu: filterMenu
    property alias densityMenu: densityMenu
    property alias columnMenu: columnMenu

    canRefresh: true
    onRefreshTriggered: root.sourceModel.select()

    model: tableViewModel
    delegate: TableViewItemDelegate {}

    resizableColumns: true
    pointerNavigationEnabled: true

    TableViewModel {
        id: tableViewModel
        delayed: true
        sourceModel: root.sourceModel
        columnModel: root.columnModel
    }

    columnWidthProvider: function(column) {
        var w = tableViewModel.columnWidth(column)
        if(w >= 0)
            return w;
        return root.defaultColumnWidthProvider(column)
    }

    selectionModel: SelectionModel {
        model: root.model
        currentIndexEnabled: true
        currentIndexPolicy: SelectionModel.CurrentIndexPolicyToggleRow | SelectionModel.CurrentIndexPolicyClear
    }

    horizontalHeaderDelegate: TableViewSortHeader {
        sortIndex: root.sortColumn
        sortOrder: root.sortOrder
        clickable: root.sortable && root.columnModel.isColumnSortable(column)
        onClicked: {
            if(root.sortColumn===column) {
                if(root.sortOrder===Qt.AscendingOrder)
                    root.sortOrder = Qt.DescendingOrder
                else {
                    root.sortOrder = Qt.AscendingOrder
                    root.sortColumn = -1
                }
            }
            else {
                root.sortOrder = Qt.AscendingOrder
                root.sortColumn = column
            }
        }
    }

    header: RowContainer {
        topPadding: root.headerPadding
        leftPadding: root.headerPadding
        rightPadding: root.headerPadding

        BarDateRange {
            id: dateRange
            visible: root.dateVisible
        }

        LayoutSpring {}

        SvgColorImage {
            visible: root.sourceModel.selectQueued
            color: Style.colorWhite
            size: 16
            icon: MaterialIcons.asterisk
        }

        TextButton {
            text: qsTr("Refresh")
            onClicked: root.refreshTriggered()
        }

        BasicSeparator {}

        TableFilterMenuButton {
            id: filterMenu
            visible: root.filterVisible
            columnModel: root.columnModel
        }

        TableDensityMenuButton {
            id: densityMenu
            tableView: root
        }

        TableColumnMenuButton {
            id: columnMenu
            model: root.columnModel
        }
    }

    footer: TablePaginationControl {
        bottomInset: root.footerPadding
        leftInset: root.footerPadding
        rightInset: root.footerPadding

        sourceModel: root.sourceModel
        visible: root.paginated
    }

}
