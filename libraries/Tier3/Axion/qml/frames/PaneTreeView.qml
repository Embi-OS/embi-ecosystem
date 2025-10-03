import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

BasicPane {
    id: root

    property AbstractItemModel model: null
    property alias delegate: view.delegate
    property alias view: view

    property alias boundsBehavior: view.boundsBehavior
    property alias reuseItems: view.reuseItems
    property alias columnWidthProvider: view.columnWidthProvider
    property alias rowHeightProvider: view.rowHeightProvider
    property alias viewWidth: view.viewWidth
    property alias viewHeight: view.viewHeight

    property alias selectionBehavior: view.selectionBehavior
    property alias selectionModel: view.selectionModel
    property alias pointerNavigationEnabled: view.pointerNavigationEnabled
    property alias currentColumn: view.currentColumn
    property alias currentRow: view.currentRow
    property alias rows: view.rows
    property alias columns: view.columns
    property alias rootIndex: view.rootIndex

    function forceLayout() {
        view.forceLayoutThrottled()
    }

    function defaultColumnWidthProvider(column: int): real {
        return view.defaultColumnWidthProvider(column);
    }

    function positionViewAtRow(row: int) {
        view.positionViewAtRow(row, TableView.Contain)
    }

    function positionViewAtColumn(column: int) {
        view.positionViewAtColumn(column, TableView.Contain)
    }

    function positionViewAtCell(row: int, column: int) {
        view.positionViewAtCell(Qt.point(row, column), TableView.Contain)
    }

    function positionViewAtCurrentRow() {
        root.positionViewAtRow(root.currentRow)
    }

    function positionViewAtCurrentColumn() {
        root.positionViewAtColumn(root.currentColumn)
    }

    function index(row: int, column: int): var {
        return view.index(row, column)
    }

    contentItem: BasicTreeView {
        id: view

        model: root.model
        rowSpacing: root.spacing
        columnSpacing: root.spacing

        ScrollBar.vertical: BasicScrollBar {
            visible: view.contentHeight>view.height
        }
    }
}
