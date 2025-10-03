pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.HorizontalHeaderView {
    id: root

    implicitWidth: syncView ? syncView.width : 0
    implicitHeight: visible ? Math.max(1, contentHeight) : 0

    clip: syncView.clip
    reuseItems: false
    resizableColumns: true
    resizableRows: false
    pointerNavigationEnabled: false
    boundsBehavior: syncView.boundsBehavior

    delegate: BasicTableViewHeaderDelegate {}
}
