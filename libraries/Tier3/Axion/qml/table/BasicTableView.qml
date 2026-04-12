pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

Item {
    id: root

    readonly property TableView tableView: view
    readonly property TableHorizontalHeaderView horizontalHeader: horizontalHeader
    readonly property TableVerticalHeaderView verticalHeader: verticalHeader

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

    readonly property bool dragging: view.dragging || horizontalHeader.dragging || verticalHeader.dragging
    readonly property bool flicking: view.flicking || horizontalHeader.flicking || verticalHeader.flicking
    readonly property bool moving: view.moving || horizontalHeader.moving || verticalHeader.moving

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
    Component.onDestruction: isCompleted=false

    onCellHeightChanged: root.queueForceLayout()
    onCellWidthChanged: root.queueForceLayout()

    Timer {
        id: forceLayoutThrottler
        interval: 0
        onTriggered: {
            if(root.isColumnWidthInSync() && root.isRowHeightInSync())
                return;

            root.isLayouting = true
            view.forceLayout()
            root.isLayouting = false
        }
    }

    function queueForceLayout() {
        if(visible && isCompleted && !isLayouting) {
            forceLayoutThrottler.start();
        }
    }

    function isColumnWidthInSync(): bool {
        for (let c = 0; c < view.columns; ++c) {
            if (!view.isColumnLoaded(c) || !horizontalHeader.isColumnLoaded(c))
                continue

            let expectedW = view.columnWidthProvider(c)
            if (expectedW !== view.columnWidth(c) || expectedW !== horizontalHeader.columnWidth(c)) {
                return false
            }
        }
        return true
    }

    function isRowHeightInSync(): bool {
        for (let r = 0; r < view.rows; ++r) {
            if (!view.isRowLoaded(r) || !verticalHeader.isRowLoaded(r))
                continue

            let expectedH = view.rowHeightProvider(r)
            if (expectedH !== view.rowHeight(r) || expectedH !== verticalHeader.rowHeight(r)) {
                return false
            }
        }
        return true
    }

    function defaultColumnWidthProvider(column: int): real {
        var w = view.explicitColumnWidth(column)
        if (w >= 0) {
            return w;
        }
        if(root.fixedCellWidth) {
            return root.cellWidth;
        }
        w = horizontalHeader.visible ? Math.max(view.implicitColumnWidth(column), horizontalHeader.implicitColumnWidth(column)) : view.implicitColumnWidth(column);
        w = Math.max(Math.min(w, root.cellWidthMaximum), root.cellWidth);
        return w;
    }

    function defaultRowHeightProvider(row: int): real {
        var h = view.explicitRowHeight(row)
        if (h >= 0) {
            return h;
        }
        if(root.fixedCellHeight) {
            return root.cellHeight;
        }
        h = verticalHeader.visible ? Math.max(view.implicitRowHeight(row), verticalHeader.implicitRowHeight(row)) : view.implicitRowHeight(row);
        h = Math.max(h, root.cellHeight);
        return h;
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

    TableHorizontalHeaderView {
        id: horizontalHeader
        anchors.left: view.left
        anchors.right: view.right
        anchors.top: parent.top
        syncView: view
        onLayoutChanged: root.queueForceLayout()

        rowHeightProvider: function (row) {
            let h = explicitRowHeight(row)
            if (h >= 0)
                return h;
            return Math.max(implicitRowHeight(row), root.cellHeight);
        }
    }

    TableVerticalHeaderView {
        id: verticalHeader
        anchors.left: parent.left
        anchors.top: horizontalHeader.bottom
        anchors.topMargin: view.rowSpacing
        anchors.bottom: view.bottom
        syncView: view
        onLayoutChanged: root.queueForceLayout()

        columnWidthProvider: function(column) {
            let w = explicitColumnWidth(column)
            if (w >= 0)
                return w;
            return Math.max(Math.min(implicitColumnWidth(column), root.cellWidthMaximum), root.cellWidth);
        }
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

        onLayoutChanged: root.queueForceLayout()
        columnWidthProvider: root.defaultColumnWidthProvider
        rowHeightProvider: root.defaultRowHeightProvider
        selectionBehavior: TableView.SelectionDisabled
        pointerNavigationEnabled: true
        keyNavigationEnabled: false
        model: root.model

        rowSpacing: 1
        columnSpacing: 0

        clip: true
        interactive: true
        reuseItems: true
        resizableColumns: false
        resizableRows: false
        editTriggers: TableView.NoEditTriggers
        flickableDirection: Flickable.HorizontalAndVerticalFlick

        delegate: TableViewLabelDelegate {
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
}
