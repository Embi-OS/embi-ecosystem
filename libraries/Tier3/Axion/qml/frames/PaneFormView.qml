pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion

PaneTreeView {
    id: root

    required property FormObjectModel formModel
    property QtObject dataObject: null
    property string formComment: ""
    property bool directEdit: false

    readonly property FormEditControlBar formEditControl: formEditControlLoader.status === Loader.Ready
                                                            ? formEditControlLoader.item as FormEditControlBar
                                                            : null
    readonly property bool warning: root.formModel.warning

    Navigation.guarded: root.formEditControl
                        && root.formEditControl.state !== ""
                        && root.formModel.isDirty

    model: proxyModel
    borderColor: root.warning ? Style.colorWarning : Style.colorPrimary
    comment: root.warning ? qsTr("Certains champs sont invalides") : root.formComment

    Binding {
        when: root.dataObject
        target: root.formModel
        property: "target"
        value: root.dataObject
    }

    ProxyModel {
        id: proxyModel
        delayed: true
        sourceModel: root.formModel
        filterRoleName: "visible"
        filterValue: true
    }

    Component {
        id: formEditControlComponent

        FormEditControlBar {
            formModel: root.formModel
            editable: root.editable
        }
    }

    Loader {
        id: formEditControlLoader
        active: !root.directEdit
        sourceComponent: formEditControlComponent
    }

    footer: root.formEditControl

    Binding {
        when: root.directEdit
        target: root.formModel
        property: "editable"
        value: root.editable
    }
}
