import QtQuick
import Eco.Tier3.Axion

BasicTreeDelegate {
    id: root

    property string info

    icon.source: hasChildren ? expanded ? MaterialIcons.chevronDown :
                                          MaterialIcons.chevronRight : ""
    icon.color: Style.colorTransparent
    font: hasChildren ? Style.textTheme.title2 : Style.textTheme.headline5

    contentItem: RowLayout {
        SvgColorImage {
            Layout.alignment: Qt.AlignVCenter
            size: 24
            icon: root.icon.source
            color: root.icon.color
        }

        Text {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            font: root.font
            color: root.color
            maximumLineCount: 1
            elide: Text.ElideRight
            text: root.text
        }

        Text {
            Layout.alignment: Qt.AlignVCenter
            font: Style.textTheme.headline7
            maximumLineCount: 1
            elide: Text.ElideRight
            color: root.color
            text: root.info
        }
    }
}
