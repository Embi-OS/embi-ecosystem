import QtQuick
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Settings
import Eco.Tier3.Axion

InputPanel {
    id: root

    z: Infinity
    // parent: Overlay.overlay

    visible: inputPanelTransition.running || root.active

    property bool hide: false
    readonly property BasicWindow rootWindow: Window.window as BasicWindow
    readonly property double windowWidth: rootWindow?.contentWidth ?? rootWindow?.width ?? 0
    readonly property double windowHeight: rootWindow?.contentHeight ?? rootWindow?.height ?? 0
    readonly property int defaultOffset: (windowHeight / 2 + height / 2)

    readonly property Item inputItem: InputContext.priv.inputItem as Item

    property list<string> activeLocales: ["de_CH","fr_CH","it_CH","en_GB"]
    property int visibleFunctionKeys: QtVirtualKeyboard.KeyboardFunctionKeys.Hide
    property bool closeOnReturn: true
    property bool handwritingModeDisabled: true
    property int inputMethodHints: Qt.ImhNone | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText

    width: windowWidth
    anchors.centerIn: parent
    anchors.verticalCenterOffset: defaultOffset

    onActiveChanged: {
        if(root.inputItem && !active)
            root.inputItem.focus = false
    }

    AutoScroller {
        id: autoScroller
        enabled: root.visible
    }

    states: State {
        name: "visible"
        when: root.active && !root.hide
        PropertyChanges {
            target: root
            anchors.verticalCenterOffset: root.defaultOffset - root.height
        }
    }

    transitions: Transition {
        id: inputPanelTransition
        from: ""
        to: "visible"
        reversible: true
        ParallelAnimation {
            NumberAnimation {
                properties: "anchors.verticalCenterOffset"
                duration: 250
                easing.type: Easing.InOutQuad
            }
        }
    }

    Binding {
        target: InputContext
        property: "animating"
        value: inputPanelTransition.running
    }

    Binding {
        target: VirtualKeyboardSettings
        property: "activeLocales"
        value: root.activeLocales
    }

    Binding {
        target: VirtualKeyboardSettings
        property: "visibleFunctionKeys"
        value: root.visibleFunctionKeys
    }

    Binding {
        target: VirtualKeyboardSettings
        property: "closeOnReturn"
        value: root.closeOnReturn
    }

    Binding {
        target: VirtualKeyboardSettings
        property: "handwritingModeDisabled"
        value: root.handwritingModeDisabled
    }

    Binding {
        target: VirtualKeyboardSettings
        property: "inputMethodHints"
        value: root.inputMethodHints
    }
}
