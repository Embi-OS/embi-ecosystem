pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.Network

PaneListView {
    id: root

    required property NetworkSettingsInterface networkInterface
    property NetworkSettingsService connectingService: null
    property bool retryConnectAfterIdle: false

    signal connectClicked(NetworkSettingsService service)
    signal settingsClicked(NetworkSettingsService service)

    onConnectClicked: (service) => {
        if(!service)
            return;
        if(service.type!==NetworkSettingsType.Wifi)
            return;

        if(service.state===NetworkSettingsState.Online || service.state===NetworkSettingsState.Ready) {
            service.disconnectService();
        } else {
            root.connectingService = service
            if (root.connectingService) {
                root.connectingService.connectService();
                root.connectingService.stateChanged.connect(root.connectingServiceStateChange);
            }
        }
    }

    onSettingsClicked: (service) => {
        if(!service)
            return;
        if(service.state===NetworkSettingsState.Online || service.state===NetworkSettingsState.Ready) {
            root.showNetworkServiceSetting(service)
        }
    }

    Connections {
        target: NetworkSettingsManager.userAgent
        function onShowUserCredentialsInput() {
            var settings = {
                "title": qsTr("Connexion"),
                "label": qsTr("Mot de passe"),
                "onInputAccepted": function(value) {
                    NetworkSettingsManager.userAgent.setPassphrase(value)
                },
                "onRejected": function() {
                    NetworkSettingsManager.userAgent.cancelInput()
                }
            }
            DialogManager.showInput(settings);
        }
    }

    function connectingServiceStateChange() {
        if (root.connectingService) {
            if (root.connectingService.state === NetworkSettingsState.Failure) {
                // If authentication failed, request connection again. That will
                // initiate new passphrase request.
                retryConnectAfterIdle = true
            } else if (root.connectingService.state === NetworkSettingsState.Ready) {
                // If connection succeeded, we no longer have service connecting
                root.connectingService = null;
                retryConnectAfterIdle = false;
            } else if (root.connectingService.state === NetworkSettingsState.Idle) {
                if (retryConnectAfterIdle) {
                    DialogManager.showWarning(qsTr("Mot de passe invalide"));
                    root.connectingService.connectService();
                }
                retryConnectAfterIdle = false;
            }
        }
    }

    function showNetworkServiceSetting(service: NetworkSettingsService) {
        var component = Qt.createComponent("Eco.Tier3.Network", "Form_NetworkServiceSetting");
        var model = component.createObject(root, {"networkService": service});
        var settings = {
            "formModel": model,
            "onClosed": function() {
                model.destroy();
            },
            "onFormValidated": function(formValues) {
                service.setupIpv4Config()
            }
        }

        DialogManager.showForm(settings);
    }

    SortFilterProxyModel {
        id: proxyModel
        delayed: true
        sourceModel: NetworkSettingsManager.services
        filterRoleName: "type"
        filterValue: root.networkInterface ? root.networkInterface.type : NetworkSettingsType.Unknown
        sorters: [RoleSorter {
            roleName: "connected"
            sortOrder: Qt.DescendingOrder
        },
        RoleSorter {
            roleName: "signalStrength"
            sortOrder: Qt.DescendingOrder
        }]
    }

    model: proxyModel

    viewSection.property: "connected"
    viewSection.delegate: BasicLabel {
        width: (ListView.view as BasicListView).viewWidth
        height: contentHeight + 2*Style.contentRectangleBorderWidth
        verticalAlignment: Text.AlignVCenter
        required property bool section
        text: section ? qsTr("Réseau(x) actuel(s):") : qsTr("Réseau(x) disponible(s):")
        font: Style.textTheme.title2
    }

    header: NetworkInterfaceDelegate {
        clickable: false
        enabled: root.editable
        netFace: root.networkInterface
        type: netFace?.type ?? NetworkSettingsType.Unknown
        connectionState: netFace?.state ?? NetworkSettingsState.Undefined
        name: netFace?.name ?? ""
        powered: netFace?.powered ?? false
    }

    delegate: NetworkServiceDelegate {
        width: (ListView.view as BasicListView).viewWidth
        required property NetworkSettingsService entry
        required name
        required identifier
        required iface
        required type
        required state
        required signalStrength
        required connected
        required mac
        connectionState: entry?.state ?? NetworkSettingsState.Undefined

        canEdit: root.editable

        service: entry

        onConnectClicked: (service) => root.connectClicked(service)
        onSettingsClicked: (service) => root.settingsClicked(service)
    }
}
