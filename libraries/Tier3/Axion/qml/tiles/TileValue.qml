import QtQuick
import Eco.Tier3.Axion

BasicTile {
    id: root

    property double value
    property string valueUnit: ""
    property int valueDecimal: 1
    property color valueColor: foregroundColor
    property font valueFont: Style.textTheme.title1

    contentItem: ColumnLayout {
        visible: !root.concealed
        spacing: root.spacing
        BasicLabel {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            opacity: 1.0
            font: root.font
            text: root.text
            color: root.foregroundColor
            wrapMode: Text.Wrap
        }

        BasicLabel {
            Layout.fillWidth: true
            Layout.fillHeight: true
            font: root.valueFont
            text: FormatUtils.realToString(root.value, root.valueDecimal)
            color: root.valueColor
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }

        BasicLabel {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            opacity: 1.0
            font: root.font
            text: ("[%1]").arg(root.valueUnit)
            color: root.foregroundColor
        }
    }
}
