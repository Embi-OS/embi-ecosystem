import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

BasicPane {
    id: root

    padding: borderWidth

    readonly property BasicTableView view: view
    readonly property TableView tableView: view.tableView
    readonly property TableHorizontalHeaderView horizontalHeader: view.horizontalHeader
    readonly property TableVerticalHeaderView verticalHeader: view.verticalHeader

    property AbstractItemModel model: null
    property alias delegate: view.delegate
    property alias horizontalHeaderDelegate: view.horizontalHeaderDelegate
    property alias verticalHeaderDelegate: view.verticalHeaderDelegate

    property alias boundsBehavior: view.boundsBehavior
    property alias reuseItems: view.reuseItems
    property alias rowSpacing: view.rowSpacing
    property alias columnSpacing: view.columnSpacing
    property alias fixedCellHeight: view.fixedCellHeight
    property alias cellHeight: view.cellHeight
    property alias fixedCellWidth: view.fixedCellWidth
    property alias cellWidth: view.cellWidth
    property alias cellWidthMaximum: view.cellWidthMaximum

    property alias selectionBehavior: view.selectionBehavior
    property alias selectionModel: view.selectionModel
    property alias pointerNavigationEnabled: view.pointerNavigationEnabled
    property alias interactive: view.interactive
    property alias currentColumn: view.currentColumn
    property alias currentRow: view.currentRow
    property alias rows: view.rows
    property alias columns: view.columns

    property alias resizableColumns: view.resizableColumns
    property alias resizableRows: view.resizableRows
    property alias canRefresh: view.canRefresh

    property alias columnWidthProvider: view.columnWidthProvider
    property alias rowHeightProvider: view.rowHeightProvider

    signal refreshTriggered()

    function forceLayout() {
        view.queueForceLayout()
    }

    function defaultColumnWidthProvider(column: int): real {
        return view.defaultColumnWidthProvider(column);
    }

    function defaultRowHeightProvider(row: int): real {
        return view.defaultRowHeightProvider(row);
    }

    function positionViewAtRow(row: int) {
        view.positionViewAtRow(row)
    }

    function positionViewAtColumn(column: int) {
        view.positionViewAtColumn(column)
    }

    function positionViewAtCell(row: int, column: int) {
        view.positionViewAtCell(row, column)
    }

    function positionViewAtCurrentRow() {
        view.positionViewAtCurrentRow()
    }

    function positionViewAtCurrentColumn() {
        view.positionViewAtCurrentColumn()
    }

    contentItem: BasicTableView {
        id: view
        model: root.model
        onRefreshTriggered: root.refreshTriggered()
    }
}
