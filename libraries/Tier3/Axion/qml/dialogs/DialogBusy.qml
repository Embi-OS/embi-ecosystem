import QtQuick
import Eco.Tier3.Axion

BasicDialog {
    id: root

    title: qsTr("Patienter")
    message: qsTr("Une tâche est en cours...")
    property string infos
    property string traces
    property string buttonReject: ""

    property bool rejectVisible: buttonReject!==""

    property bool indicatorEnabled: true
    property bool progressEnabled: true
    property real progress: 0

    buttonsContainer: [
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.RejectRole; visible: root.rejectVisible; text: root.buttonReject; highlighted: true }
    ]

    header: RowContainer{
        topPadding: root.headerPadding
        leftPadding: root.headerPadding
        rightPadding: root.headerPadding
        alignment: Qt.AlignTop

        LabelWithCaption {
            Layout.fillHeight: true
            Layout.fillWidth: true

            horizontalAlignment: root.headerHorizontalAlignment
            verticalAlignment: root.headerVerticalAlignment

            visible: haveAText || haveACaption

            text: root.title
            textFont: Style.textTheme.title1

            caption: root.message
            captionFont: Style.textTheme.subtitle1
        }

        BasicBusyIndicator {
            visible: root.indicatorEnabled
        }
    }

    contentItem: ColumnLayout {
        spacing: root.spacing

        LabelWithCaption {
            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: root.spacing
            relativeBackgroundColor: root.color
            textFont: root.font
            text: root.infos
            textMaximumLineCount: 0
            captionFont: root.font
            caption: root.traces
            captionMaximumLineCount: 0
        }

        BasicProgressBar {
            Layout.fillWidth: true

            value: root.progress
            from: 0
            to: 10000
            visible: root.progressEnabled && root.progress>=0
        }
    }
}
