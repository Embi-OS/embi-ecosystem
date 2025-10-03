pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

RowContainer {
    id: root

    font: Style.textTheme.headline6

    required property AbstractItemModel sourceModel
    property string paginationRole: "pagination"

    readonly property int offset: (pagination.page-1)*pagination.perPage
    readonly property int limit: pagination.perPage
    readonly property int startRow: root.offset+1
    readonly property int endRow: root.offset+pagination.count

    Binding {
        when: root.visible && root.sourceModel && root.sourceModel.hasOwnProperty(root.paginationRole)
        target: root.sourceModel
        property: root.paginationRole
        value: SyncablePagination {
            id: pagination
            page: 1
            perPage: verbComboBox.currentValue
        }
    }

    FormComboBox {
        id: verbComboBox
        font: root.font
        fitLabel: true
        implicitWidth: 280
        showLabelRectangle: false
        colorControlBackground: Style.colorTransparent

        label: qsTr("Lignes par page")
        placeholder: qsTr("Lignes")
        options: ListModel {
            ListElement { text: "25"; value: 25 }
            ListElement { text: "50"; value: 50 }
            ListElement { text: "100"; value: 100 }
            ListElement { text: "250"; value: 250 }
            ListElement { text: "500"; value: 500 }
            // ListElement { text: qsTr("Tout"); value: 0 }
        }
        textRole: "text"
        valueRole: "value"
    }

    LayoutSpring {}

    BasicLabel {
        visible: pagination.pageCount>1
        font: root.font
        text: qsTr("Lignes %1 à %2 de %3").arg(root.startRow).arg(root.endRow).arg(pagination.total)
    }

    BasicLabel {
        visible: pagination.pageCount<=1
        font: root.font
        text: qsTr("%1 lignes").arg(pagination.total)
    }

    BasicSeparator {}

    RawButton {
        enabled: pagination.page>1
        icon.source: MaterialIcons.pageFirst
        onClicked: pagination.page = 1
    }

    RawButton {
        enabled: pagination.page>1
        icon.source: MaterialIcons.chevronLeft
        onClicked: pagination.page = pagination.page-1
    }

    BasicLabel {
        font: root.font
        text: qsTr("Page %1 / %2").arg(pagination.page).arg(pagination.pageCount)
    }

    RawButton {
        enabled: pagination.page<pagination.pageCount
        icon.source: MaterialIcons.chevronRight
        onClicked: pagination.page = pagination.page+1
    }

    RawButton {
        enabled: pagination.page<pagination.pageCount
        icon.source: MaterialIcons.pageLast
        onClicked: pagination.page = pagination.pageCount
    }
}
