import QtQuick
import Eco.Tier3.Axion
import Eco.Tier3.Files

BasicItemDelegate {
    id: root

    property string info

    property bool canEdit: false
    property bool canEject: false

    property int checkState: Qt.Unchecked

    signal folderClicked()
    signal ejectClicked()

    contentItem: RowLayout {
        spacing: root.spacing

        BasicCheckIndicator {
            Layout.alignment: Qt.AlignVCenter
            visible: root.checkable
            checkState: root.checkState
        }

        SvgColorImage {
            Layout.alignment: Qt.AlignVCenter
            icon: root.icon.source
            iconWidth: Style.roundToIconSize(root.icon.width)
            iconHeight: Style.roundToIconSize(root.icon.height)
            color: root.icon.color
            cache: root.icon.cache
        }

        LabelWithCaption {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            color: root.foregroundColor
            text: root.text
            textFont: Style.textTheme.title2
            caption: root.info
            captionFont: Style.textTheme.subtitle1
        }

        RawButton {
            Layout.alignment: Qt.AlignVCenter
            visible: !root.checkable
            color: Style.colorPrimaryLight
            icon.source: MaterialIcons.folderOpenOutline
            onClicked: root.folderClicked()
        }

        RawButton {
            Layout.alignment: Qt.AlignVCenter
            visible: root.canEject && !root.checkable
            color: Style.colorPrimaryLight
            icon.source: MaterialIcons.ejectOutline
            onClicked: root.ejectClicked()
        }
    }
}
