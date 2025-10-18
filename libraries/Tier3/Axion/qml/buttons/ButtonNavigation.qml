import QtQuick
import Eco.Tier3.Axion

Rectangle {
    id: root

    required property int position
    required property bool isFirstRow

    property bool navigationLocked: false

    signal clicked(int position)

    readonly property bool isFirstColumn: !position
    property int buttonHeight: Style.buttonNavigationSize
    property int buttonWidth: buttonHeight
    property int rightMargin: 15

    property string icon: ""
    property string text: ""
    property bool highlighted: false
    readonly property color iconColor: highlighted ? Style.barNavigationBorderColorHighlight : Style.colorWhite
    readonly property int leftShift: 2*root.radius //root.border.width*2 + root.radius

    border.width: Style.barNavigationBorderWidth
    border.color: Style.barNavigationBorderColor
    radius: Style.barNavigationBorderRadius
    color: root.highlighted ? Style.barNavigationBorderColorHighlight : Style.barNavigationColor

    implicitWidth: (isFirstColumn ? root.radius : leftShift) + buttonWidth + (label.visible ? label.width + rightMargin : 0)
    implicitHeight: (isFirstRow ? root.radius : 0) + buttonHeight

    BasicRectangularGlow {
        id: circle
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: Math.ceil((root.isFirstRow ? root.radius-root.border.width : 0)/2)
        anchors.left: parent.left
        anchors.leftMargin: Math.ceil((root.isFirstColumn ? root.radius-root.border.width : root.leftShift) + (root.buttonWidth - width) / 2)

        height: Style.buttonNavigationCircleSize
        width: height

        color: Style.colorBlack
        glowRadius: Style.buttonNavigationGlow
        spread: 0.75
        cornerRadius: (height/2) + glowRadius

        SvgColorImage {
            anchors.centerIn: parent
            size: Style.buttonNavigationIconSize
            color: root.iconColor
            icon: root.icon
        }
    }

    Text {
        id: label
        visible: text !== ""
        anchors.top: parent.top
        anchors.topMargin: (root.isFirstRow ? root.radius : 0) + (root.buttonHeight - height) / 2
        anchors.right: parent.right
        anchors.rightMargin: root.rightMargin

        color: Style.colorBlack
        font: Style.textTheme.title2
        text: root.text
    }

    TapHandler {
        id: mouseArea
        grabPermissions: PointerHandler.TakeOverForbidden
        gesturePolicy: TapHandler.ReleaseWithinBounds
        onTapped: {
            if(!root.navigationLocked)
                root.clicked(root.position)
            else
                SnackbarManager.showWarning(qsTr("La navigation est désactivée"))
        }
    }
}
