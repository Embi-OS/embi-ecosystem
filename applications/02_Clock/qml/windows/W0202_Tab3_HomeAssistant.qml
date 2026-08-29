pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.Hass

BasicPane {
    id: root

    readonly property string connectionState: {
        if (homeAssistantSocket.accessToken.length === 0)
            return qsTr("Jeton Home Assistant manquant")
        if (homeAssistantSocket.subscribed)
            return qsTr("Synchronisation temps réel active")
        if (homeAssistantSocket.authenticated)
            return qsTr("Abonnement aux événements en cours")
        return qsTr("Connexion WebSocket en cours")
    }
    readonly property string connectionError: homeAssistantSocket.protocolError.length > 0
                                            ? homeAssistantSocket.protocolError
                                            : homeAssistantSocket.error

    padding: 20
    topPadding: 0

    HassSocket {
        id: homeAssistantSocket

        connection: "hass"
        accessToken: HassManager.apiToken
        enabled: root.visible && accessToken.length > 0
    }

    HassEntityModel {
        id: entityModel

        HassSocketAttached.socket: homeAssistantSocket
        connection: "hass"
    }

    ProxyModel {
        id: entityProxyModel
        sourceModel: entityModel
        sortRoleName: "entity_id"
        sortOrder: Qt.AscendingOrder
    }

    contentItem: PaneGridView {
        rowSpacing: 5
        columnSpacing: 5
        itemWidth: 220
        itemHeight: 80

        model: entityProxyModel

        delegate: HassEntityDelegateChooser {
            onEntityClicked: (index) => console.log(FormatUtils.variantToLog(entityModel.at(entityProxyModel.mapToSource(index))))
        }

        header: RowLayout {
            LabelWithCaption {
                Layout.fillWidth: true
                textFont: Style.textTheme.subtitle2
                text: qsTr("Home Assistant: %1").arg(HassManager.apiEffectiveUrl)
                captionFont: Style.textTheme.caption1
                caption: root.connectionState+" "+root.connectionError
            }

            FabButton {
                icon.source: MaterialIcons.pen
                hint: qsTr("Configurer")
                onClicked: {
                    var component = Qt.createComponent("Eco.Tier3.Hass", "Form_SettingsHass");
                    var model = component.createObject(root) as FormObjectModel;
                    DialogManager.showForm({
                        "formModel": model,
                        "formObject": HassManager,
                        "onClosed": function() {
                            model.destroy()
                            component.destroy()
                        }
                    })
                }
            }
        }
    }
}
