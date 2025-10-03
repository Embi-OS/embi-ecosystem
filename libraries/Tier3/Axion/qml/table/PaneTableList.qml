import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion

PaneTableCheckable {
    id: root

    property alias tableViewModel: tableViewModel
    property alias proxyModel: proxyModel
    property alias sortProxyModel: sortProxyModel
    required property AbstractItemModel sourceModel
    required property TableViewColumnModel columnModel
    property Component headerSecondaryBar: null

    readonly property int currentSourceRow: mapToSource(currentRow)

    function mapToSource(row: int): int {
        return proxyModel.mapToSource(sortProxyModel.mapToSource(row));
    }

    function mapFromSource(row: int): int {
        return sortProxyModel.mapFromSource(proxyModel.mapFromSource(row));
    }

    property bool refreshable: false
    canRefresh: refreshable

    model: sortProxyModel
    delegate: TableViewItemDelegate {}

    columnWidthProvider: function(column) {
        var w = tableViewModel.columnWidth(column)
        if(w >= 0)
            return w;
        return root.defaultColumnWidthProvider(column)
    }

    header: BasicTableListHeader {
        topInset: root.headerPadding
        leftInset: root.headerPadding
        rightInset: root.headerPadding

        tableView: root
        secondaryBar: root.headerSecondaryBar
        title: root.title
        comment: root.comment
        columnModel: root.columnModel
        refreshable: root.refreshable
        onRefreshClicked: root.refreshTriggered()
    }

    footer: BasicLabel {
        bottomInset: root.footerPadding
        leftInset: root.footerPadding
        rightInset: root.footerPadding

        text: qsTr("Nb. data: %1").arg(root.rows)
        font: Style.textTheme.subtitle1
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
        sortColumn: root.sortColumn
        sortOrder: root.sortOrder
        sortRoleName: "display"
    }
}
