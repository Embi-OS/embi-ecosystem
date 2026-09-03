import QtQuick
import Eco.Tier3.Axion
import L02_Clock

PaneFormView {
    id: root

    directEdit: true
    title: qsTr("Gestion du Pi-Clock")
    formModel: FormObjectModel {
        id: treeModel
        editable: root.editable

        FormSwitchDelegate {
            label: qsTr("Always on")
            target: ClockDisplay
            targetProperty: "alwaysOnEnabled"
        }

        FormSliderDelegate {
            label: qsTr("Volume par défaut")
            infos: qsTr("Volume utilisé par les alarmes sans volume personnalisé et plafond appliqué au déverrouillage")
            from: 0
            to: 100
            stepSize: 1
            target: ClockAlarmAudio
            targetProperty: "defaultVolume"
        }

        FormSliderDelegate {
            label: qsTr("Fondu d'entrée par défaut")
            infos: qsTr("Durée en secondes utilisée par les alarmes sans fondu d'entrée personnalisé")
            from: 0
            to: 60
            stepSize: 1
            target: ClockAlarmAudio
            targetProperty: "defaultFadeInDuration"
        }

        FormSliderDelegate {
            label: qsTr("Arrêt automatique par défaut")
            infos: qsTr("Durée en minutes avant l'arrêt automatique des alarmes sans valeur personnalisée ; 0 impose un arrêt manuel")
            from: 0
            to: 120
            stepSize: 1
            target: Clock
            targetProperty: "ringTimeout"
        }

        FormSliderDelegate {
            label: qsTr("Réveil écran par défaut")
            infos: qsTr("Durée en minutes utilisée pour réveiller l'écran des alarmes sans valeur personnalisée")
            from: 0
            to: 60
            stepSize: 1
            target: Clock
            targetProperty: "wakeTimeout"
        }

        FormSliderDelegate {
            label: qsTr("Snooze par défaut")
            infos: qsTr("Durée en minutes utilisée lorsque le snooze est activé sans durée personnalisée")
            from: 1
            to: 60
            stepSize: 1
            target: Clock
            targetProperty: "snoozeTimeout"
        }
    }
}
