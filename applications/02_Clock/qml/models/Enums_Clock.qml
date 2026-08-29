import QtQml
import Eco.Tier1.Models
import L02_Clock

QtObject {
    id: root

    component AlarmAudioSourcesModel: StandardObjectModel {
        StandardObject { value: ClockAlarmAudioSources.MediaPlayer; text: qsTr("Lecteur média") }
        StandardObject { value: ClockAlarmAudioSources.Ringtone; text: qsTr("Sonnerie") }
    }

    component AlarmRingtonesModel: StandardObjectModel {
        StandardObject { value: "qrc:/ringtones/ACH_Homecoming.ogg"; text: "Homecoming" }
        StandardObject { value: "qrc:/ringtones/ACH_Chime_Time.ogg"; text: "Chime Time" }
        StandardObject { value: "qrc:/ringtones/ACH_Feels_Good.ogg"; text: "Feels Good" }
    }

    component AlarmMediaStartModesModel: StandardObjectModel {
        StandardObject { value: ClockAlarmMediaStartModes.Current; text: qsTr("Morceau actuel") }
        StandardObject { value: ClockAlarmMediaStartModes.Next; text: qsTr("Morceau suivant") }
    }
}
