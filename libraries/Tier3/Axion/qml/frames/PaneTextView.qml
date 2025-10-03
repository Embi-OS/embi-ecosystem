import QtQuick
import Eco.Tier3.Axion

BasicPane {
    id: root

    font: Style.textTheme.code
    property alias text: textArea.text
    property alias textFormat: textArea.textFormat
    property alias view: view

    contentItem: BasicScrollView {
        id: view
        BasicTextArea {
            id: textArea
            readOnly: true
            wrapMode: Text.Wrap
            textFormat: TextEdit.PlainText
            font: root.font
            relativeBackgroundColor: root.color
        }
    }
}
