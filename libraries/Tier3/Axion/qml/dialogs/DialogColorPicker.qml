import QtQuick
import Eco.Tier3.Axion

BasicDialog {
    id: root

    signal colorSelected(color color)

    property alias selectedColor: colorPicker.selectedColor
    property alias solidColor: colorPicker.solidColor
    property alias hueColor: colorPicker.hueColor
    property alias rgbColor: colorPicker.rgbColor
    property alias colorModel: colorPicker.colorModel

    property alias paletteEnabled: colorPicker.paletteEnabled
    property alias saturationEnabled: colorPicker.saturationEnabled
    property alias hueEnabled: colorPicker.hueEnabled
    property alias alphaEnabled: colorPicker.alphaEnabled
    property alias swatchEnabled: colorPicker.swatchEnabled
    property alias hexEnabled: colorPicker.hexEnabled

    property string buttonReject: qsTr("Annuler")
    property string buttonAccept: qsTr("Valider")

    title: qsTr("Couleur")
    message: qsTr("Sélectionner une couleur")

    onAccepted: root.colorSelected(root.rgbColor)

    buttonsContainer: [
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole; text: root.buttonAccept; highlighted: true},
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.RejectRole; text: root.buttonReject}
    ]

    preferredWidth: 0
    preferredHeight: 0
    padding: Style.contentRectangleBorderWidth

    contentItem: Flickable {
        implicitHeight: colorPicker.implicitHeight
        implicitWidth: colorPicker.implicitWidth
        contentHeight: Math.max(height, colorPicker.height)
        flickableDirection: Flickable.AutoFlickIfNeeded
        clip: true

        ColorPicker {
            id: colorPicker

            width: parent.width
            implicitHeight: 400

            alphaEnabled: false
            swatchEnabled: false
        }
    }
}
