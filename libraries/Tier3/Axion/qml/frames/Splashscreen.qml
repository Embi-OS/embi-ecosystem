import QtQuick
import QsLog
import Eco.Tier3.Axion

BasicPane {
    id: root

    property string logo: Style.splash

    LogDestModel {
        id: logModel
        limit: 1
        enabled: root.visible
    }

    background: Background {
        SvgColorImage {
            anchors.centerIn: parent
            asynchronous: false
            color: Style.colorWhite
            icon: root.logo
            iconWidth: root.width * 0.7
            iconHeight: root.height * 0.7
            aspectRatio: Qt.KeepAspectRatio
        }
    }

    footer: ColumnLayout {
        BasicBusyIndicator {
            Layout.alignment: Qt.AlignHCenter
        }

        BasicLabel {
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Qt.AlignHCenter
            font: Style.textTheme.body1
            text: logModel.lastMessage.display
            maximumLineCount: 1
        }
    }
}
