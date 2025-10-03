pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.VerticalHeaderView {
    id: root

    implicitWidth: visible ? Math.max(1, contentWidth) : 0
    implicitHeight: syncView ? syncView.height : 0

    clip: syncView.clip
    reuseItems: syncView.reuseItems
    resizableColumns: false
    resizableRows: false
    pointerNavigationEnabled: false
    boundsBehavior: syncView.boundsBehavior

    delegate: BasicTableViewHeaderDelegate {}
}
