import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import Eco.Tier3.Network

FormObjectModel {
    id: root

    required property NetworkSettingsService networkService
    readonly property NetworkSettingsIPv4 ipv4: networkService.ipv4
    readonly property NetworkSettingsIPv6 ipv6: networkService.ipv6
    readonly property NetworkSettingsProxy proxy: networkService.proxy
    readonly property NetworkSettingsAddressModel domains: networkService.domains as NetworkSettingsAddressModel
    readonly property NetworkSettingsAddressModel nameservers: networkService.nameservers as NetworkSettingsAddressModel
    readonly property NetworkSettingsAddressModel timeservers: networkService.timeservers as NetworkSettingsAddressModel
    readonly property NetworkSettingsEthernet ethernetConfig: networkService.ethernetConfig
    readonly property NetworkSettingsWireless wirelessConfig: networkService.wirelessConfig

    FormInfoDelegate {
        label: qsTr("Nom")
        target: root.networkService
        targetProperty: "name"
    }
    FormInfoDelegate {
        label: qsTr("Interface")
        target: root.ethernetConfig
        targetProperty: "interface"
    }
    FormInfoDelegate {
        label: qsTr("Adresse IPv4")
        target: root.ipv4
        targetProperty: "address"
    }
    FormInfoDelegate {
        label: qsTr("Adresse IPv6")
        target: root.ipv6
        targetProperty: "address"
    }
    FormInfoDelegate {
        label: qsTr("Adresse matérielle")
        target: root.ethernetConfig
        targetProperty: "address"
    }
    FormInfoDelegate {
        label: qsTr("Domaine")
        value: root.domains.ModelHelper.getProperties("display").join(", ")
    }
    FormInfoDelegate {
        label: qsTr("Route par défaut")
        target: root.ipv4
        targetProperty: "gateway"
    }
    FormInfoDelegate {
        label: qsTr("DNS")
        value: root.nameservers.ModelHelper.getProperties("display").join(", ")
    }

    SeparatorTreeDelegate {}

    FormSwitchDelegate {
        enabled: root.networkService.type===NetworkSettingsType.Wifi
        label: qsTr("Connexion automatique")
        target: root.networkService
        targetProperty: "autoConnect"
    }

    FormNestDelegate {
        text: qsTr("IPv4")

        FormComboBoxDelegate {
            id: methodIPv4
            label: qsTr("Méthode IPv4")
            options: StandardObjectModel {
                StandardObject {text: qsTr("Automatique (DHCP)"); value: NetworkSettingsIPv4.Dhcp }
                StandardObject {text: qsTr("Manuel"); value: NetworkSettingsIPv4.Manual }
                StandardObject {text: qsTr("Désactiver"); value: NetworkSettingsIPv4.Off }
            }
            textRole: "text"
            valueRole: "value"
            target: root.ipv4
            targetProperty: "method"
        }
        FormTextFieldDelegate {
            visible: methodIPv4.currentValue===NetworkSettingsIPv4.Manual
            label: qsTr("Adresse")
            validator: Ipv4Validator {}
            target: root.ipv4
            targetProperty: "address"
        }
        FormTextFieldDelegate {
            visible: methodIPv4.currentValue===NetworkSettingsIPv4.Manual
            label: qsTr("Masque de réseau")
            validator: Ipv4Validator {}
            target: root.ipv4
            targetProperty: "mask"
        }
        FormTextFieldDelegate {
            visible: methodIPv4.currentValue===NetworkSettingsIPv4.Manual
            label: qsTr("Passerelle")
            validator: Ipv4Validator {}
            target: root.ipv4
            targetProperty: "gateway"
        }
    }

    FormNestDelegate {
        text: qsTr("Domaines")

        FormSwitchDelegate {
            id: autoDomain
            label: qsTr("Automatique")
            value: root.domains.automatic
            onAccepted: (value) => {
                if(value) {
                    root.domains.clear()
                    root.networkService.setupDomainsConfig()
                }
            }
        }

        FormTextFieldDelegate {
            enabled: !autoDomain.checked
            fitLabel: true
            placeholder: qsTr("Noms de domaines")
            infos: qsTr("Séparer les noms de domaines avec des virgules")
            value: root.domains.ModelHelper.getProperties("display").join(", ")
            onAccepted: (value) => {
                if(autoDomain.checked)
                    return;
                var addresses = value.split(/\s*,\s*/)
                root.domains.changeStringList(addresses)
                root.networkService.setupDomainsConfig()
            }
        }
    }

    FormNestDelegate {
        text: qsTr("DNS")

        FormSwitchDelegate {
            id: autoDns
            label: qsTr("Automatique")
            value: root.nameservers.automatic
            onAccepted: (value) => {
                if(value) {
                    root.nameservers.clear()
                    root.networkService.setupNameserversConfig()
                }
            }
        }

        FormTextFieldDelegate {
            enabled: !autoDns.checked
            fitLabel: true
            placeholder: qsTr("Adresses IP")
            infos: qsTr("Séparer les adresses IP avec des virgules")
            value: root.nameservers.ModelHelper.getProperties("display").join(", ")
            onAccepted: (value) => {
                if(autoDns.checked)
                    return;
                var addresses = value.split(/\s*,\s*/)
                root.nameservers.changeStringList(addresses)
                root.networkService.setupNameserversConfig()
            }
        }
    }
}
