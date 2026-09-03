import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import L02_Clock

FormObjectModel {
    id: root

    Component.onDestruction: ClockAlarmAudio.stopRingtonePreview()

    FormComboBoxDelegate {
        id: audioSourceField
        label: qsTr("Source audio")
        infos: qsTr("Choisissez entre le lecteur média ou une sonnerie dédiée")
        options: Enums_Clock.AlarmAudioSourcesModel {}
        textRole: "text"
        valueRole: "value"
        defaultValue: ClockAlarmAudioSources.MediaPlayer
        targetProperty: "audio.source"
    }

    FormComboBoxDelegate {
        id: ringtoneField
        visible: audioSourceField.currentValue === ClockAlarmAudioSources.Ringtone
        label: qsTr("Sonnerie")
        infos: qsTr("La sonnerie est jouée en boucle jusqu'à l'arrêt de l'alarme ; un aperçu est lancé à chaque sélection")
        options: Enums_Clock.AlarmRingtonesModel {}
        textRole: "text"
        valueRole: "value"
        targetProperty: "audio.ringtone"

        onCurrentValueChanged: {
            if(!root.selecting && currentValue)
                ClockAlarmAudio.playRingtonePreview(currentValue, audioVolumeField.currentValue)
        }
    }

    FormButtonDelegate {
        visible: ringtoneField.visible && ClockAlarmAudio.ringtonePreviewing
        label: qsTr("Arrêter l'aperçu")
        onClicked: ClockAlarmAudio.stopRingtonePreview()
    }

    FormSliderDelegate {
        id: audioVolumeField
        label: qsTr("Volume")
        infos: qsTr("Volume maximum de l'alarme ; il peut être réduit lorsque l'écran est déverrouillé pendant le fondu")
        from: 0
        to: 100
        stepSize: 1
        defaultValue: ClockAlarmAudio.defaultVolume
        targetProperty: "audio.volume"
    }

    FormSliderDelegate {
        label: qsTr("Fondu d'entrée")
        infos: qsTr("Durée du fondu d'entrée en secondes")
        from: 0
        to: 120
        stepSize: 1
        defaultValue: ClockAlarmAudio.defaultFadeInDuration
        targetProperty: "audio.fadeInDuration"
    }

    FormComboBoxDelegate {
        visible: audioSourceField.currentValue === ClockAlarmAudioSources.MediaPlayer
        label: qsTr("Démarrage du lecteur média")
        infos: qsTr("Actuel reprend le média sélectionné ; Suivant avance dans la playlist")
        options: Enums_Clock.AlarmMediaStartModesModel {}
        textRole: "text"
        valueRole: "value"
        defaultValue: ClockAlarmMediaStartModes.Next
        targetProperty: "audio.mediaStartMode"
    }

    FormSliderDelegate {
        label: qsTr("Arrêt automatique (min)")
        infos: qsTr("0 désactive l'arrêt automatique")
        from: 0
        to: 120
        stepSize: 1
        defaultValue: Clock.ringTimeout
        targetProperty: "behavior.autoDismissDuration"
    }

    FormSliderDelegate {
        label: qsTr("Durée du réveil écran (min)")
        infos: qsTr("0 applique immédiatement le niveau de luminosité de réveil")
        from: 0
        to: 60
        stepSize: 1
        defaultValue: Clock.wakeTimeout
        targetProperty: "display.wakeTimeout"
    }

    FormSwitchDelegate {
        id: snoozeEnabledField
        label: qsTr("Snooze")
        infos: qsTr("Permet de repousser une alarme en cours au lieu de l'arrêter")
        defaultValue: false
        targetProperty: "behavior.snoozeEnabled"
    }

    FormSliderDelegate {
        visible: snoozeEnabledField.checked
        label: qsTr("Durée snooze (min)")
        infos: qsTr("Durée du report lorsque le snooze est activé")
        from: 1
        to: 60
        stepSize: 1
        defaultValue: Clock.snoozeTimeout
        targetProperty: "behavior.snoozeDuration"
    }
}
