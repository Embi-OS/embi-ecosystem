import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.MenuSeparator {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    Layout.fillWidth: true

    property double separatorLargePadding: Style.separatorLargePadding
    property double separatorSmallPadding: Style.separatorSmallPadding
    property double separatorImplicitWidth: Style.separatorImplicitWidth
    property double separatorSize: Style.separatorSize

    horizontalPadding: root.separatorSmallPadding
    verticalPadding: root.separatorLargePadding

    property bool highlighted: false
    property color color: Style.colorPrimaryLight
    property color foregroundColor: highlighted ? Style.colorAccent : color

    contentItem: Rectangle {
        implicitWidth: root.separatorImplicitWidth
        implicitHeight: root.separatorSize
        color: root.foregroundColor
        radius: Math.min(width, height)/2
    }
}
