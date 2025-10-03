import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.DialogButtonBox {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             visibleCount>0 ? contentHeight + topPadding + bottomPadding : 0)

    spacing: Style.dialogButtonSpacing
    padding: Style.dialogButtonPadding
    alignment: Qt.AlignRight
    buttonLayout: T.DialogButtonBox.AndroidLayout

    delegate: ButtonDialog {}

    readonly property int visibleCount: itemContainer.visibleCount

    onCountChanged: {
        for(const child of contentChildren) {
            child.ItemContainer.container = itemContainer
        }
    }

    ItemContainer {
        id: itemContainer
        delayed: true
    }

    contentItem: ListView {
        implicitWidth: root.contentWidth
        model: root.contentModel
        spacing: root.spacing
        orientation: ListView.Horizontal
        boundsBehavior: Flickable.StopAtBounds
        snapMode: ListView.SnapToItem
    }
}
