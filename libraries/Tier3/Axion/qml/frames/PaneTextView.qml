pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

BasicPane {
    id: root

    font: Style.textTheme.code1
    property string text: ""
    property int textFormat: Text.PlainText
    property color textColor: ColorUtils.isDarkColor(root.color) ? Style.colorWhite : Style.colorBlack

    TextListModel {
        id: model
        text: root.text
    }

    contentItem: BasicListView {
        id: view
        model: model
        delegate: Text {
            required property string display
            padding: 0
            width: ListView.view.width
            wrapMode: Text.Wrap
            text: display
            textFormat: root.textFormat
            font: root.font
            color: root.textColor
        }

        ScrollBar.vertical: BasicScrollBar {
            visible: view.contentHeight>view.height
        }
        ScrollBar.horizontal: BasicScrollBar {
            visible: view.contentWidth>view.width
        }
    }

    // contentItem: BasicScrollView {
    //     id: view
    //     BasicTextArea {
    //         id: textArea
    //         readOnly: true
    //         wrapMode: Text.Wrap
    //         text: root.text
    //         textFormat: root.textFormat
    //         font: root.font
    //         color: root.textColor
    //     }
    // }
}
