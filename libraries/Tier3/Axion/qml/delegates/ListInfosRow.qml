import QtQuick
import Eco.Tier3.Axion

Item {
    id: root

    property bool fitLabel: false
    property double labelWidthRatio: 0.5
    property double labelWidth: width * labelWidthRatio
    property double inset: 0

    property int type: InfoObjectTypes.Normal
    property string info: ""
    property string label: ""
    property font font: Style.textTheme.subtitle1

    property color color: type===InfoObjectTypes.Normal ? Style.colorWhite :
                          type===InfoObjectTypes.Warning ? Style.colorWarning :
                          type===InfoObjectTypes.Error ? Style.colorError :
                          type===InfoObjectTypes.Failure ? Style.colorFatal :
                          type===InfoObjectTypes.Success ? Style.colorSuccess :
                          Style.colorWhite

    implicitHeight: Math.max(labelLabel.height, infoLabel.height)

    Text {
        id: labelLabel
        anchors.left: parent.left
        anchors.leftMargin: root.inset
        anchors.top: parent.top
        width: root.fitLabel ? undefined : root.labelWidth
        font: root.font
        color: Style.colorWhite
        wrapMode: Text.Wrap
        text: root.label
    }

    Text {
        id: separator
        anchors.top: parent.top
        anchors.left: labelLabel.right
        font: root.font
        color: Style.colorWhite
        text: (root.info==="" || root.label==="") ? "  " : ": "
    }

    Text {
        id: infoLabel
        anchors.top: parent.top
        anchors.left: separator.right
        anchors.right: parent.right
        anchors.rightMargin: root.inset
        font: root.font
        color: root.color
        wrapMode: Text.Wrap
        text: root.info
    }
}
