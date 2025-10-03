pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Utils
import Eco.Tier1.Models
import Eco.Tier3.Axion

Item {
    id: root

    property alias tableView: view
    property alias horizontalHeader: horizontalHeader
    property alias verticalHeader: verticalHeader

    property AbstractItemModel model: null
    property alias delegate: view.delegate
    property alias horizontalHeaderDelegate: horizontalHeader.delegate
    property alias verticalHeaderDelegate: verticalHeader.delegate

    property alias boundsBehavior: view.boundsBehavior
    property alias reuseItems: view.reuseItems
    property alias rowSpacing: view.rowSpacing
    property alias columnSpacing: view.columnSpacing
    property alias columnWidthProvider: view.columnWidthProvider
    property alias rowHeightProvider: view.rowHeightProvider

    property alias selectionBehavior: view.selectionBehavior
    property alias selectionModel: view.selectionModel
    property alias pointerNavigationEnabled: view.pointerNavigationEnabled
    property alias interactive: view.interactive
    property alias currentColumn: view.currentColumn
    property alias currentRow: view.currentRow
    property alias rows: view.rows
    property alias columns: view.columns

    property alias resizableColumns: horizontalHeader.resizableColumns
    property alias resizableRows: verticalHeader.resizableRows

    property bool canRefresh: false

    property bool fixedCellHeight: true
    property double cellHeight: Style.tableCellHeightStandard

    property bool fixedCellWidth: false
    property double cellWidth: Style.tableCellWidth
    property double cellWidthMaximum: Style.tableCellWidthMaximum

    signal refreshTriggered()

    property bool isLayouting: false
    property bool isCompleted: false
    Component.onCompleted: isCompleted=true

    onCellHeightChanged: root.forceLayoutThrottled()
    onCellWidthChanged: root.forceLayoutThrottled()

    SignalTrailingDebouncer {
        id: forceLayoutThrottler
        timeout: 0
        onTriggered: {
            root.isLayouting = true
            view.forceLayout()
            root.isLayouting = false
        }
    }

    function forceLayoutThrottled() {
        if(isCompleted && !isLayouting && view.rows>0)
            forceLayoutThrottler.throttle();
    }

    function defaultColumnWidthProvider(column: int): real {
        var w = view.explicitColumnWidth(column)
        if (w >= 0) {
            return w;
        }
        if(root.fixedCellWidth) {
            return root.cellWidth;
        }
        if(!view.isColumnLoaded(column) || (horizontalHeader.visible && !horizontalHeader.isColumnLoaded(column))) {
            return -1;
        }
        w = horizontalHeader.visible ? Math.max(view.implicitColumnWidth(column), horizontalHeader.implicitColumnWidth(column)) : view.implicitColumnWidth(column);
        if(w<view.columnWidth(column) || (horizontalHeader.visible && w<horizontalHeader.columnWidth(column))) {
            root.forceLayoutThrottled()
        }
        return Math.max(Math.min(w, root.cellWidthMaximum), root.cellWidth);
    }

    function defaultRowHeightProvider(row: int): real {
        var h = view.explicitRowHeight(row)
        if (h >= 0) {
            return h;
        }
        if(root.fixedCellHeight) {
            return root.cellHeight;
        }
        if(!view.isRowLoaded(row) || (verticalHeader.visible && !verticalHeader.isRowLoaded(row))) {
            return -1;
        }
        h = verticalHeader.visible ? Math.max(view.implicitRowHeight(row), verticalHeader.implicitRowHeight(row)) : view.implicitRowHeight(row);
        if(h<view.rowHeight(row) || (verticalHeader.visible && h<verticalHeader.rowHeight(row))) {
            root.forceLayoutThrottled()
        }
        return Math.max(h, root.cellHeight);
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

    TableView {
        id: view
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.left: verticalHeader.right
        anchors.leftMargin: columnSpacing
        anchors.top: horizontalHeader.bottom
        anchors.topMargin: rowSpacing
        anchors.rightMargin: (ScrollBar.vertical.visible ? columnSpacing+ScrollBar.vertical.width : 0)
        anchors.bottomMargin: (ScrollBar.horizontal.visible ? rowSpacing+ScrollBar.horizontal.height : 0)

        columnWidthProvider: root.defaultColumnWidthProvider
        rowHeightProvider: root.defaultRowHeightProvider
        selectionBehavior: TableView.SelectionDisabled
        pointerNavigationEnabled: true
        keyNavigationEnabled: false
        model: root.model

        rowSpacing: 1 //0
        columnSpacing: 0 //0

        clip: true
        interactive: true
        reuseItems: true
        resizableColumns: false
        resizableRows: false
        editTriggers: TableView.NoEditTriggers
        flickableDirection: Flickable.AutoFlickIfNeeded

        delegate: BasicTableViewDelegate {
            required property string display
            text: display
        }

        ScrollBar.vertical: BasicScrollBar{
            // visible: view.contentHeight>view.height
            parent: view.parent
            anchors.top: view.top
            anchors.left: view.right
            anchors.leftMargin: view.columnSpacing
            anchors.bottom: view.bottom
            anchors.topMargin: -(horizontalHeader.height+view.rowSpacing)
        }
        ScrollBar.horizontal: BasicScrollBar {
            // visible: view.contentWidth>view.width
            parent: view.parent
            anchors.right: view.right
            anchors.left: view.left
            anchors.top: view.bottom
            anchors.topMargin: view.rowSpacing
            anchors.leftMargin: -(verticalHeader.width+view.columnSpacing)
        }

        Loader {
            id: busyIndicatorLoader
            active: root.canRefresh
            sourceComponent: PullToRefreshHandle {
                parent: view
                onTriggered: root.refreshTriggered()
            }
        }
    }

    TableHorizontalHeaderView {
        id: horizontalHeader
        anchors.left: view.left
        anchors.right: view.right
        anchors.top: parent.top
        syncView: view
        onLayoutChanged: {
            if(!visible)
                return;
            root.forceLayoutThrottled()
        }
        rowHeightProvider: function (row) {
            let h = explicitRowHeight(row)
            if (h >= 0)
                return h;
            return root.cellHeight;
        }
    }

    TableVerticalHeaderView {
        id: verticalHeader
        anchors.left: parent.left
        anchors.top: horizontalHeader.bottom
        anchors.topMargin: view.rowSpacing
        anchors.bottom: view.bottom
        syncView: view
        onLayoutChanged: {
            if(!visible)
                return;
            root.forceLayoutThrottled()
        }
        rowHeightProvider: function (row) {
            let h = explicitRowHeight(row)
            if (h >= 0)
                return h;
            return root.cellHeight;
        }
        columnWidthProvider: function(column) {
            let w = explicitColumnWidth(column)
            if (w >= 0)
                return w;
            return Math.max(implicitColumnWidth(column), root.cellWidth);
        }
    }
}
