import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.MenuBar {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding)

    property color backgroundColor: Style.colorPrimaryDark
    spacing: Style.menuItemSpacing

    delegate: BasicMenuBarItem {}

    contentItem: Row {
        spacing: root.spacing
        Repeater {
            model: root.contentModel
        }
    }

    background: Rectangle {
        implicitHeight: Style.menuItemMinHeight
        color: root.backgroundColor
    }
}
