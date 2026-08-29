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
        from: 0
        to: 100
        stepSize: 1
        defaultValue: 20
        targetProperty: "audio.volume"
    }

    FormSliderDelegate {
        label: qsTr("Fondu d'entrée")
        from: 0
        to: 60
        stepSize: 1
        defaultValue: 0
        targetProperty: "audio.fadeInDuration"
    }

    FormComboBoxDelegate {
        visible: audioSourceField.currentValue === ClockAlarmAudioSources.MediaPlayer
        label: qsTr("Démarrage du lecteur média")
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
        defaultValue: 30
        targetProperty: "behavior.autoDismissDuration"
    }

    FormSliderDelegate {
        label: qsTr("Durée du réveil écran (min)")
        from: 0
        to: 60
        stepSize: 1
        defaultValue: 5
        targetProperty: "display.wakeTimeout"
    }

    FormSwitchDelegate {
        id: snoozeEnabledField
        label: qsTr("Snooze")
        defaultValue: false
        targetProperty: "behavior.snoozeEnabled"
    }

    FormSliderDelegate {
        visible: snoozeEnabledField.checked
        label: qsTr("Durée snooze (min)")
        from: 1
        to: 60
        stepSize: 1
        defaultValue: 5
        targetProperty: "behavior.snoozeDuration"
    }
}
