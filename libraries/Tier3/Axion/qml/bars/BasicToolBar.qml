pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.ToolBar {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding)

    spacing: Style.toolBarSpacing

    property color backgroundColor: Style.colorPrimary
    property double elevation: Style.toolBarElevation

    background: Rectangle {
        implicitHeight: Style.toolBarImplicitHeight
        color: root.backgroundColor

        layer.enabled: root.elevation>0
        layer.effect: BasicElevationEffect {
            elevation: root.elevation
        }
    }
}
