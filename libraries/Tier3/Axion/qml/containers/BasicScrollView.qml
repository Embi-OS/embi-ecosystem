import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.ScrollView {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding)

    ScrollBar.vertical: BasicScrollBar {
        visible: root.contentHeight>root.height
        parent: root
        x: root.mirrored ? 0 : root.width - width
        y: root.topPadding
        height: root.availableHeight
        active: root.ScrollBar.horizontal.active
    }

    ScrollBar.horizontal: BasicScrollBar {
        visible: root.contentWidth>root.width
        parent: root
        x: root.leftPadding
        y: root.height - height
        width: root.availableWidth
        active: root.ScrollBar.vertical.active
    }
}
