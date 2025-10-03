import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.System

PaneTreeView {
    id: root

    readonly property LocaleFilterModel localeModel: LocaleFilterModel {
        filters: [
            "C",
            "fr_CH",
            "de_CH",
            "it_CH",
            "en_GB"
        ]
    }

    locale: Qt.locale(LocaleSettings.locale)

    title: qsTr("Réglage de la langue")

    model: proxyModel

    ProxyModel {
        id: proxyModel
        delayed: true
        sourceModel: treeModel
        filterRoleName: "visible"
        filterValue: true
    }

    FormObjectModel {
        id: treeModel
        editable: root.editable
        target: LocaleSettings

        FormComboBoxDelegate {
            visible: LocaleSettings.canSetLocale
            label: qsTr("Locale")
            infos: qsTr("Réglage langue et format")
            options: root.localeModel
            isSorted: true
            textRole: "display"
            valueRole: "name"
            targetProperty: "locale"
            onAccepted: AxionHelper.warningRestart()
        }
        SeparatorTreeDelegate {}

        InfoTreeDelegate {
            text: qsTr("Language");
            info: root.locale.nativeLanguageName
        }
        InfoTreeDelegate {
            text: qsTr("Monnaie");
            info: root.locale.currencySymbol(Locale.CurrencyDisplayName)
        }
        InfoTreeDelegate {
            text: qsTr("Monnaie");
            info: root.locale.currencySymbol(Locale.CurrencySymbol)
        }
        InfoTreeDelegate {
            text: qsTr("Monnaie");
            info: root.locale.currencySymbol(Locale.CurrencyIsoCode)
        }
        InfoTreeDelegate {
            text: qsTr("Format date");
            info: root.locale.dateFormat(Locale.LongFormat)
        }
        InfoTreeDelegate {
            text: qsTr("Format date court");
            info: root.locale.dateFormat(Locale.ShortFormat)
        }
        InfoTreeDelegate {
            text: qsTr("Format heure");
            info: root.locale.timeFormat(Locale.LongFormat)
        }
        InfoTreeDelegate {
            text: qsTr("Format heure court");
            info: root.locale.timeFormat(Locale.ShortFormat)
        }
    }
}
