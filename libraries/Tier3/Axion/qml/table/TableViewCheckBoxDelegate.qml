import QtQuick
import Eco.Tier3.Axion

BasicTableViewDelegate {
    id: root

    property alias tristate: checkBox.tristate
    property alias checkable: checkBox.checkable
    property alias checked: checkBox.checked
    property alias checkState: checkBox.checkState
    property alias nextCheckState: checkBox.nextCheckState

    signal canceled()
    signal clicked()
    signal doubleClicked()
    signal pressAndHold()
    signal pressed()
    signal released()
    signal toggled()

    contentItem: BasicCheckBox {
        id: checkBox
        tristate: false
        padding: 0

        onCanceled: root.canceled()
        onClicked: root.clicked()
        onDoubleClicked: root.doubleClicked()
        onPressAndHold: root.pressAndHold()
        onPressed: root.pressed()
        onReleased: root.released()
        onToggled: root.toggled()
    }
}
