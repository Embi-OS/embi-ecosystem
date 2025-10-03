pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

PaneTableView {
    id: root

    property bool checkable: true
    readonly property bool hasSelection: selectionModel.hasSelection
    readonly property bool hasCurrent: view.currentRow>=0

    property bool sortable: true
    property int sortColumn: -1
    property int sortOrder: Qt.AscendingOrder

    verticalHeader.visible: checkable && rows>0

    resizableColumns: true
    pointerNavigationEnabled: !selectionModel.hasSelection

    onCurrentRowChanged: root.positionViewAtCurrentRow()
    onRowsChanged: root.positionViewAtCurrentRow()

    selectionModel: SelectionModel {
        model: root.model
        currentIndexEnabled: true
        currentIndexPolicy: SelectionModel.CurrentIndexPolicyToggleRow | SelectionModel.CurrentIndexPolicyClear
    }

    delegate: TableViewItemDelegate {}

    horizontalHeaderDelegate: TableViewSortHeader {
        sortIndex: root.sortColumn
        sortOrder: root.sortOrder
        clickable: root.sortable
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

    verticalHeader.selectionModel: ItemSelectionModel {}
    verticalHeaderDelegate: TableViewCheckBoxDelegate {
        backgroundImplicitWidth: Style.tableCellHeight
        backgroundImplicitHeight: Style.tableCellHeight
        current: isRowCurrent
        selected: isRowSelected
        checkState: selected ? Qt.Checked : Qt.Unchecked

        readonly property bool isRowCurrent: root.selectionModel.currentIndex.row===row
        readonly property bool isRowSelected: root.selectionModel.SelectionHelper.hasSelection && root.selectionModel.isRowSelected(row)

        onClicked: {
            root.selectionModel.SelectionHelper.setRow(row, checked)
        }
    }

    TableViewCheckBoxHeader {
        parent: root.contentItem
        anchors.left: root.verticalHeader.left
        anchors.right: root.verticalHeader.right
        anchors.top: root.horizontalHeader.top
        anchors.bottom: root.horizontalHeader.bottom
        visible: root.verticalHeader.visible

        selectionModel: root.selectionModel
        syncView: root.tableView
    }
}
