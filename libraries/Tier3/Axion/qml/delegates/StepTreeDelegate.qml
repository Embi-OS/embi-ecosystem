pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

StandardObject {
    id: root

    unfolded: status===StepObjectStates.Current

    text: ""
    hint: ""
    value: -1
    font: Style.textTheme.headline7
    icon: root.status===StepObjectStates.Current ? MaterialIcons.arrowRightThin :
          root.status===StepObjectStates.Done ? MaterialIcons.check :
          root.status===StepObjectStates.Failed ? MaterialIcons.close :
          root.status===StepObjectStates.Skipped ? MaterialIcons.arrowURightBottom : MaterialIcons.template

    property int status: StepObjectStates.NotDone

    delegate: IconLabel {
        opacity: 1.0

        icon.source: root.icon
        icon.width: 24
        icon.height: 24
        icon.color: Style.colorWhite

        alignment: Qt.AlignLeft | Qt.AlignVCenter
        font: root.font
        text: root.text
        wrapMode: Text.NoWrap
        elide: Text.ElideRight
    }
}
