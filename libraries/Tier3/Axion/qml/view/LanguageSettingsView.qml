import QtQuick
import Eco.Tier3.Axion

PaneFormView {
    id: root

    directEdit: true
    locale: Qt.locale()

    title: qsTr("Réglage de la langue")

    formModel: FormObjectModel {
        id: treeModel
        editable: root.editable
        target: LanguageSettings

        FormComboBoxDelegate {
            label: qsTr("Langue")
            infos: qsTr("Langue utilisée par l'application")
            options: LanguageModel {}
            textRole: "display"
            valueRole: "name"
            targetProperty: "language"
        }

        SeparatorTreeDelegate {}

        InfoTreeDelegate {
            text: qsTr("Langue active")
            info: LanguageSettings.effectiveLanguage
        }

        SeparatorTreeDelegate {}
        InfoTreeDelegate {
            text: qsTr("Locale active");
            info: root.locale.name
        }
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
