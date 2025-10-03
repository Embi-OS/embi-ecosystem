pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.Popup {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding)

    property double inset: Style.popupInset
    topInset: inset
    bottomInset: inset
    leftInset: inset
    rightInset: inset

    padding: Math.max(radius, borderWidth) + inset
    spacing: Style.popupSpacing

    font: Style.textTheme.body2

    modal: true // Back darker
    focus: true
    closePolicy: T.Popup.CloseOnEscape | T.Popup.CloseOnPressOutside

    enter: Transition {
        NumberAnimation { property: "scale"; from: 0.9; to: 1.0; easing.type: Easing.OutQuint; duration: 220 }
        NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; easing.type: Easing.OutCubic; duration: 150 }
    }

    exit: Transition {
        NumberAnimation { property: "scale"; from: 1.0; to: 0.9; easing.type: Easing.OutQuint; duration: 220 }
        NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; easing.type: Easing.OutCubic; duration: 150 }
    }

    T.Overlay.modal: BasicOverlay {
        Behavior on opacity { NumberAnimation { duration: 150 } }
    }

    T.Overlay.modeless: BasicOverlay {
        Behavior on opacity { NumberAnimation { duration: 150 } }
    }

    property double elevation: Style.popupElevation
    property color color: Style.colorPrimary
    property color borderColor: Style.colorPrimary
    property double borderWidth: Style.popupBorderWidth
    property double radius: Style.popupRadius

    background: Item {
        layer.enabled: root.elevation>0
        layer.effect: BasicElevationEffect {
            elevation: root.elevation
        }

        Rectangle {
            anchors.fill: parent
            radius: root.radius
            color: root.color
            border.width: root.borderWidth
            border.color: root.borderColor
        }
    }
}
