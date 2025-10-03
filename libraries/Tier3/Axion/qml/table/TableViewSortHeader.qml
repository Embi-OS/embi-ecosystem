pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion

BasicTableViewHeaderDelegate {
    id: root

    property int sortIndex: -1
    property int sortOrder: Qt.AscendingOrder
    property string sortIcon: root.sortOrder===Qt.AscendingOrder ? MaterialIcons.sortAscending : MaterialIcons.sortDescendingVariant

    highlighted: sortIndex===column
    icon.source: highlighted ? sortIcon :
                 clickable ? MaterialIcons.unfoldMoreHorizontal : ""
}
