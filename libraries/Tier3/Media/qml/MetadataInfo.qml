pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Media
import Eco.Tier3.Axion

Rectangle {
    id: root

    property alias view: view

    required property int mediaStatus
    required property mediaMetaData metaData

    color: ColorUtils.transparent(Style.colorPrimaryDark, 0.8)

    MediaMetaDataModel {
        id: metaDataModel
        metaData: root.metaData
        selectWhen: root.mediaStatus!==MediaPlayer.LoadingMedia
        selectPolicy: QVariantListModelPolicies.Delayed
        submitPolicy: QVariantListModelPolicies.Disabled
    }

    BasicListView {
        id: view
        visible: !root.hide
        anchors.fill: parent
        anchors.margins: 10
        model: metaDataModel

        headerPositioning: ListView.PullBackHeader
        header: RowContainer {
            inset: 5
            width: ListView.view.width
            BasicLabel {
                text: qsTr("Métaddonnées")
                Layout.fillWidth: true
                font: Style.textTheme.body1
            }
            ClickableIcon {
                icon: MaterialIcons.close
                onClicked: root.hide=true
            }
        }

        delegate: ListInfosRow {
            width: (ListView.view as BasicListView).viewWidth
            inset: 5

            required property string name
            required property string value

            font: Style.textTheme.caption1
            label: name
            info: value
        }
    }

    BasicLabel {
        visible: !metaDataModel.count
        font: Style.textTheme.caption1
        text: qsTr("No metadata present")
        anchors.centerIn: parent
    }

    TapHandler {
        onTapped: root.hide = !root.hide
    }

    property bool hide: true
    states: [
        State { when: !root.hide
            PropertyChanges { root.opacity: 1.0    }
        },
        State { when: root.hide
            PropertyChanges { root.opacity: 0.0    }
        }
    ]

    transitions: Transition {
        NumberAnimation { property: "opacity"; duration: 100}
    }
}
