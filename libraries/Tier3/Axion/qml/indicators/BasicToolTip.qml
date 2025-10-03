import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.ToolTip {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    margins: 12
    horizontalPadding: Style.toolTipPadding
    verticalPadding: 0

    closePolicy: T.Popup.CloseOnEscape | T.Popup.CloseOnPressOutsideParent | T.Popup.CloseOnReleaseOutsideParent

    font: Style.textTheme.body2

    enter: Transition {
        NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; easing.type: Easing.OutQuad; duration: 100 }
    }

    exit: Transition {
        NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; easing.type: Easing.InQuad; duration: 200 }
    }

    contentItem: Text {
        text: root.text
        font: root.font
        color: Style.colorBlack
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.NoWrap
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitHeight: Style.toolTipImplicitHeight
        color: Style.toolTipColor
        opacity: Style.toolTipOpacity
        radius: Style.toolTipRadius
    }
}
