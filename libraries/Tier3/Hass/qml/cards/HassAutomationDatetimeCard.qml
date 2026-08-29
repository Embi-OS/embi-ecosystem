import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion
import Eco.Tier3.Hass

T.AbstractButton {
    id: root

    required property HassStateMapper timeMapper
    required property HassStateMapper automationMapper

    readonly property string time_entity_id: root.timeMapper.entity_id
    readonly property string time_state: root.timeMapper.state
    readonly property var time_attributes: root.timeMapper.attributes
    readonly property bool isTimeUnavailable: time_state==="unavailable"
    readonly property bool isTimeUnknown: time_state==="unknown"
    readonly property bool hasDate: time_attributes?.has_date ?? false
    readonly property bool hasTime: time_attributes?.has_time ?? false
    readonly property date valueDateTime: stateDateTime()

    readonly property string automation_entity_id: root.automationMapper.entity_id
    readonly property string automation_state: root.automationMapper.state
    readonly property var automation_attributes: root.automationMapper.attributes
    readonly property bool isAutomationUnavailable: automation_state==="unavailable"
    readonly property bool isAutomationUnknown: automation_state==="unknown"
    readonly property bool isAutomationOn: automation_state==="on"

    function stateDateTime(): date {
        const stateParts = time_state.split(" ")
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

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    property double inset: Style.buttonInset
    topInset: inset
    bottomInset: inset
    leftInset: inset
    rightInset: inset

    padding: 24

    spacing: 8
    property int radius: 10

    icon.width: 96
    icon.height: 96
    icon.color: Style.colorBlack
    icon.source: MaterialIcons.sunClock

    property color color: Style.colorPrimary
    property color foregroundColor: ColorUtils.isDarkColor(root.color) ? Style.colorWhite : Style.colorBlack

    contentItem: LabelWithCaption {
        spacing: root.spacing

        color: root.foregroundColor
        text: root.time_attributes?.friendly_name ?? root.time_entity_id
        textFont.family: Style.textTheme.primaryFont
        textFont.pixelSize: 16
        textFont.weight: Font.Normal
        caption: {
            if(root.isTimeUnavailable || root.isTimeUnknown)
                return "N/A"
            if(root.hasDate && root.hasTime)
                return DateTimeUtils.formatDateTime(root.valueDateTime, Locale.ShortFormat)
            if(root.hasDate)
                return DateTimeUtils.formatDate(root.valueDateTime, Locale.ShortFormat)
            if(root.hasTime)
                return DateTimeUtils.formatTime(root.valueDateTime, Locale.ShortFormat)
            return root.time_state
        }
        captionFont.family: Style.textTheme.primaryFont
        captionFont.pixelSize: 48
        captionFont.weight: Font.Medium
    }

    background: Rectangle {
        implicitWidth: 240
        implicitHeight: 120

        radius: root.radius
        color: root.color

        SvgColorImage {
            anchors.top: parent.top
            anchors.topMargin: root.inset
            anchors.right: parent.right
            anchors.rightMargin: root.inset
            iconWidth: root.icon.width
            iconHeight: root.icon.height
            icon: {
                if(root.isAutomationUnavailable)
                    return MaterialIcons.alertCircleOutline
                if(root.isAutomationUnknown)
                    return MaterialIcons.helpCircleOutline
                return root.isAutomationOn ? root.icon.source : MaterialIcons.alarmOff
            }
            color: root.icon.color
            opacity: 0.3
        }

        BasicVeil {
            z: 100
            anchors.fill: parent
            radius: root.radius

            relativeBackgroundColor: root.color
            focussed: root.down || root.visualFocus || root.hovered
            pressed: root.pressed
        }
    }
}
