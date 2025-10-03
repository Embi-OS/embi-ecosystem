import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.ToolSeparator {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    Layout.fillWidth: horizontal
    Layout.fillHeight: vertical

    property double separatorLargePadding: Style.separatorLargePadding
    property double separatorSmallPadding: Style.separatorSmallPadding
    property double separatorImplicitWidth: Style.separatorImplicitWidth
    property double separatorSize: Style.separatorSize

    horizontalPadding: vertical ? root.separatorLargePadding : root.separatorSmallPadding
    verticalPadding: vertical ? root.separatorSmallPadding : root.separatorLargePadding

    property bool highlighted: false
    property color color: Style.colorPrimary
    property color foregroundColor: highlighted ? Style.colorAccent : color

    contentItem: Rectangle {
        implicitWidth: root.vertical ? root.separatorSize : root.separatorImplicitWidth
        implicitHeight: root.vertical ? root.separatorImplicitWidth : root.separatorSize
        color: root.foregroundColor
        radius: Math.min(width, height)/2
    }
}
