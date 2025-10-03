pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.Control {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    property double inset: 0
    topInset: inset
    bottomInset: inset
    leftInset: inset
    rightInset: inset

    padding: inset
    topPadding: padding + topInset
    bottomPadding: padding + bottomInset
    leftPadding: padding + leftInset
    rightPadding: padding + rightInset

    spacing: 0

    font: Style.textTheme.body1

    // ──── General ──── //
    required property StepMachine stepMachine
    readonly property StepElement currentStep: stepMachine.currentStep
    readonly property int count: stepMachine.count

    // ──── Dimensions NEEDED to display elements ──── //
    // Indicator Dimensions
    property int indicatorWidth: 48
    property int indicatorHeight: 48

    property int indicatorIconSize: 32

    property int indicatorHorizontalPadding: 4 //8
    property int indicatorLeftPadding: indicatorHorizontalPadding
    property int indicatorRightPadding: indicatorHorizontalPadding

    property int indicatorVerticalPadding: 4 //8
    property int indicatorTopPadding: indicatorVerticalPadding
    property int indicatorBottomPadding: indicatorVerticalPadding

    // Label Dimensions
    property int labelWidth: 100
    property int labelHeight: 20

    property int labelVerticalPadding: 4 //8
    property int labelHorizontalPadding: 4 //8

    // Separator Dimensions
    property int separatorWidth: Style.contentRectangleBorderWidth/2
    property int separatorHeight: Style.contentRectangleBorderWidth/2

    // ──── Delegates ──── //
    property Component indicatorDelegate: Indicator {}
    property Component labelDelegate: LabelItem {}
    property Component separatorDelegate: Separator {}

    // ──── Component resuming the step ──── //
    component Indicator: Rectangle {
        id: indicatorItem
        width: root.indicatorWidth
        height: root.indicatorHeight

        property StepElement element
        property int index
        readonly property int state: element?.state ?? StepStates.None
        readonly property string icon: element?.icon ?? ""

        radius: width / 2
        border.width: Style.contentRectangleBorderWidth/2
        border.color: state===StepStates.Started ? Style.colorAccent : Style.colorTransparent
        color: Style.colorTransparent

        SvgColorImage {
            anchors.centerIn: parent
            icon: indicatorItem.icon
            size: root.indicatorIconSize
            color: indicatorItem.state>=StepStates.Ended ? Style.colorAccent : Style.colorWhite
        }
    }

    // ──── Component representing the step displayed below the indicator ──── //
    component LabelItem: BasicLabel {
        width: root.labelWidth
        height: root.labelHeight

        property StepElement element
        property int index
        readonly property int state: element?.state ?? StepStates.None
        readonly property string label: element?.label ?? ""

        highlighted: state>=StepStates.Ended

        font: root.font
        text: label
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap
    }

    // ──── Component displayed between indicators to separate different steps ──── //
    component Separator: Rectangle {
        property StepElement previousElement
        property StepElement nextElement
        property int index
        readonly property int previousState: previousElement?.state ?? StepStates.None
        readonly property int nextState: nextElement?.state ?? StepStates.None
        readonly property bool highlighted: previousState>=StepStates.Ended && nextState>=StepStates.Started

        color: highlighted ? Style.colorAccent : Style.colorPrimary
        height: root.separatorHeight
        radius: root.separatorWidth
    }

    // ──── Private properties ──── //
    readonly property int stepperWidth: width / count
    readonly property int stepperHeight: height / count

    contentItem: Item {
        implicitWidth: root.count * (root.labelWidth + 2 * root.labelHorizontalPadding)
        implicitHeight: root.indicatorTopPadding + root.indicatorHeight + root.indicatorBottomPadding + root.labelHeight + 2 * root.labelVerticalPadding

        Repeater {
            model: root.stepMachine
            delegate: Item {
                id: stepItem

                required property StepElement qtObject
                required property int index
                required property string instruction

                x: index * root.stepperWidth
                y: 0
                height: root.height
                width: root.stepperWidth

                Loader {
                    id: indicatorLoader
                    x: root.stepperWidth / 2 - root.indicatorWidth / 2
                    y: root.indicatorTopPadding

                    sourceComponent: root.indicatorDelegate

                    Binding {
                        target: indicatorLoader.item
                        when: indicatorLoader.item
                        property: "element"
                        value: stepItem.qtObject
                    }

                    Binding {
                        target: indicatorLoader.item
                        when: indicatorLoader.item
                        property: "index"
                        value: stepItem.index
                    }

                    BasicToolTip {
                        id: toolTip
                        ExtraPosition.position: ItemPositions.Bottom
                        ExtraPosition.margins: 5
                        text: stepItem.instruction
                        delay: 50
                        visible: tapHandler.pressed && stepItem.instruction!==""
                    }
                }

                Loader {
                    id: labelLoader
                    x: root.stepperWidth / 2 - root.labelWidth / 2
                    y: root.indicatorTopPadding + root.indicatorHeight + root.indicatorBottomPadding

                    sourceComponent: root.labelDelegate

                    Binding {
                        target: labelLoader.item
                        when: labelLoader.item
                        property: "element"
                        value: stepItem.qtObject
                    }

                    Binding {
                        target: labelLoader.item
                        when: labelLoader.item
                        property: "index"
                        value: stepItem.index
                    }
                }

                TapHandler {
                    id: tapHandler
                }
            }
        }

        Repeater {
            model: Math.max(0, root.count - 1)

            delegate: Loader {
                id: separatorLoader

                required property int index

                x: root.stepperWidth / 2 + root.indicatorWidth / 2 + root.indicatorLeftPadding + index * root.stepperWidth
                y: root.indicatorTopPadding + root.indicatorHeight / 2 - root.separatorHeight / 2
                width: root.stepperWidth - root.indicatorWidth - root.indicatorLeftPadding - root.indicatorRightPadding
                height: root.separatorHeight

                sourceComponent: root.separatorDelegate

                Binding {
                    target: separatorLoader.item
                    when: separatorLoader.item
                    property: "previousElement"
                    value: root.stepMachine.at(separatorLoader.index)
                }

                Binding {
                    target: separatorLoader.item
                    when: separatorLoader.item
                    property: "nextElement"
                    value: root.stepMachine.at(separatorLoader.index + 1)
                }

                Binding {
                    target: separatorLoader.item
                    when: separatorLoader.item
                    property: "index"
                    value: separatorLoader.index
                }
            }
        }
    }
}
