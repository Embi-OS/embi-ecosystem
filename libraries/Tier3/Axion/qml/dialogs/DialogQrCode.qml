import QtQuick
import Eco.Tier3.Axion

BasicDialog {
    id: root

    property string dataToEncode: ""

    title: root.dataToEncode!=="" ? root.dataToEncode : "N/A"
    message: root.dataToEncode!=="" ? qsTr("Généré automatiquement") : qsTr("Rien à générer")

    buttonsContainer: [
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole; text: qsTr("Fermer"); highlighted: true }
    ]

    contentItem: Item {
        implicitHeight: 200
        implicitWidth: 200
        Rectangle {
            anchors.centerIn: parent
            height: 200
            width: height

            radius: Style.contentRectangleRadius
            color: Style.colorWhite

            Image {
                anchors.centerIn: parent
                sourceSize.width: parent.width * 3/4
                sourceSize.height: parent.height * 3/4

                source: "image://QrCodeImage/" + root.dataToEncode
                asynchronous: true
            }
        }
    }
}
