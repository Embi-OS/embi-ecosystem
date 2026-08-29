import QtQuick
import Eco.Tier3.Axion

BasicDialog {
    id: root

    title: qsTr("Alimentation")

    property bool quitVisible: Power.canQuit
    property bool suspendVisible: Power.canSuspend
    property bool restartVisible: Power.canRestart
    property bool rebootVisible: Power.canReboot
    property bool shutdownVisible: Power.canShutdown

    signal quit()
    signal suspend()
    signal restart()
    signal reboot()
    signal shutdown()

    onQuit: Power.quit()
    onSuspend: Power.suspend()
    onRestart: Power.restart()
    onReboot: Power.reboot()
    onShutdown: Power.shutdown()

    buttonsContainer: [
        ButtonDialog {DialogButtonBox.buttonRole: DialogButtonBox.RejectRole; id: cancelButton; text: qsTr("Annuler");},
        ButtonDialog {DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole; id: suspendButton; text: qsTr("Veille");      onClicked: root.suspend();  visible: root.suspendVisible;   highlighted: !suspendButton.visible && !restartButton.visible && !rebootButton.visible && !shutdownButton.visible},
        ButtonDialog {DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole; id: restartButton; text: qsTr("Relancer");    onClicked: root.restart();  visible: root.restartVisible;   highlighted: !restartButton.visible && !rebootButton.visible && !shutdownButton.visible},
        ButtonDialog {DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole; id: quitButton; text: qsTr("Quitter");        onClicked: root.quit();     visible: root.quitVisible;      highlighted: !rebootButton.visible && !shutdownButton.visible},
        ButtonDialog {DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole; id: rebootButton; text: qsTr("Redémarrer");   onClicked: root.reboot();   visible: root.rebootVisible;    highlighted: !shutdownButton.visible},
        ButtonDialog {DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole; id: shutdownButton; text: qsTr("Eteindre");   onClicked: root.shutdown(); visible: root.shutdownVisible;  highlighted: true}
    ]
}
