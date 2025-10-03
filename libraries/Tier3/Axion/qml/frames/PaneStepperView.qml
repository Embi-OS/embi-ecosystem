pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion

BasicPane {
    id: root

    navigationLocked: currentItem?.navigationLocked ?? false
    navigationEnabled: loader.ready && (currentItem?.navigationEnabled ?? true)

    required property StepMachine stepMachine
    readonly property StepElement currentStep: stepMachine?.currentStep ?? null
    readonly property Component currentComponent: root.currentStep?.delegate ?? emptyComponent

    property alias asynchronous: loader.asynchronous
    readonly property Item currentItem: loader.item as Item
    property alias active: loader.active

    property Component emptyComponent: Item {}

    Binding {
        target: root.currentItem
        when: root.currentItem && root.currentItem.hasOwnProperty("stepElement")
        property: "stepElement"
        value: root.currentStep
    }

    header: BasicStepper {
        topInset: root.headerPadding
        leftInset: root.headerPadding
        rightInset: root.headerPadding

        stepMachine: root.stepMachine
    }

    contentItem: ViewDelegateLoader {
        id: loader

        delayed: true
        asynchronous: true
        delegate: root.currentComponent
        active: root.isCompleted
    }
}
