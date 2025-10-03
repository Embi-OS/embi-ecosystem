pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion

Item {
    id: root

    z: Infinity

    property bool animation: true
    property bool asynchronous: false //true

    Repeater {
        model: DialogManager.children
        delegate: LoaderDialogDelegate {
            required property DialogObject qtObject

            dialogObject: qtObject
            required isVisible
            required fullscreen
            required settings
            required type
            required severity
            required title
            required message
            required infos
            required traces
            required progress

            required buttonApply
            required buttonReject
            required buttonAccept
            required buttonDiscard
            required buttonHelp

            animation: root.animation
            asynchronous: root.asynchronous
        }
    }
}
