import QtQuick
import Eco.Tier3.Axion
import Eco.Tier3.Hass

HassDelegate {
    id: root

    property string entity_id: ""
    property string state: ""
    property var attributes: ({})

    readonly property bool isUnavailable: state === "unavailable"
    readonly property bool isUnknown: state === "unknown"
    readonly property bool hasDate: attributes?.has_date ?? false
    readonly property bool hasTime: attributes?.has_time ?? false
    readonly property date valueDateTime: stateDateTime()

    function stateDateTime() {
        const stateParts = state.split(" ")
        const dateParts = hasDate ? stateParts[0].split("-") : []
        const timePart = hasDate && hasTime ? stateParts[1] : hasTime ? stateParts[0] : ""
        const timeParts = timePart.split(":")

        return new Date(hasDate ? Number(dateParts[0]) : 1970,
                        hasDate ? Number(dateParts[1]) - 1 : 0,
                        hasDate ? Number(dateParts[2]) : 1,
                        hasTime ? Number(timeParts[0]) : 0,
                        hasTime ? Number(timeParts[1]) : 0,
                        hasTime ? Number(timeParts[2]) : 0)
    }

    primaryText: attributes?.friendly_name ?? entity_id
    secondaryText: {
        if(isUnavailable)
            return qsTr("Indisponible")
        if(isUnknown)
            return qsTr("État inconnu")
        if(hasDate && hasTime)
            return DateTimeUtils.formatDateTime(valueDateTime, Locale.ShortFormat)
        if(hasDate)
            return DateTimeUtils.formatDate(valueDateTime, Locale.ShortFormat)
        if(hasTime)
            return DateTimeUtils.formatTime(valueDateTime, Locale.ShortFormat)
        return state
    }
    enabled: !isUnavailable
    iconSource: {
        if(isUnavailable)
            return MaterialIcons.alertCircleOutline
        if(isUnknown)
            return MaterialIcons.calendarQuestionOutline
        if(hasDate && hasTime)
            return MaterialIcons.calendarClockOutline
        return hasDate ? MaterialIcons.calendarOutline : MaterialIcons.clockOutline
    }
}
