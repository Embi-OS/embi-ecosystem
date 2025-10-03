import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

BasicTableViewHeaderDelegate {
    id: root

    display: "N/A"
    model: null
    row: -1
    column: -1

    required property ItemSelectionModel selectionModel
    required property TableView syncView

    contentItem: BasicCheckBox {
        tristate: true
        checkState: !root.selectionModel.SelectionHelper.hasSelection ? Qt.Unchecked :
                    root.selectionModel.SelectionHelper.count===(root.syncView.rows*root.syncView.columns) ? Qt.Checked : Qt.PartiallyChecked

        nextCheckState: function() {
            if (checkState===Qt.Unchecked)
                return Qt.Checked
            else
                return Qt.Unchecked
        }

        onClicked: {
            if (checkState===Qt.Checked)
                root.selectionModel.SelectionHelper.selectAll()
            else if (checkState===Qt.Unchecked)
                root.selectionModel.clearSelection()
        }
    }
}
