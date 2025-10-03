import QtQuick
import Eco.Tier3.Axion

FocusScope {
    id: root

    implicitWidth: 300
    implicitHeight: controlRectangle.implicitHeight + hintRectangle.implicitHeight

    opacity: root.enabled ? 1.0 : 0.4

    signal clicked()

    property bool fitLabel: false
    property double labelWidthRatio: 0.35
    property double labelWidth: width * labelWidthRatio
    property double defaultCellHeight: Style.formCellHeight
    property double cellHeight: defaultCellHeight
    property double infosHeight: Style.formInfosHeight

    property string label: ""
    property string placeholder: ""
    property string infos: ""
    property string errorMessage: ""
    readonly property string hint: (errorMessage!=="" && warning) ? errorMessage : infos

    property bool warning: false
    property bool highlighted: false
    property bool editable: true

    property bool haveALabel: !(label==="")
    property bool haveAHint: !(hint==="")
    property bool haveAnErrorMessage: !(errorMessage==="")

    property double borderWidth: Style.formBorderWidth
    property double borderRadius: Style.formBorderRadius

    property bool showLabelRectangle: Style.formShowLabelRectangle
    property color colorHighlight: Style.colorAccent
    property color colorWarning: Style.colorWarning
    property color colorValue: Style.colorWhite
    property color borderColorDefault: Style.colorPrimary
    property color colorBackground: Style.colorBackground
    property color colorControlBackground: (editable && enabled) ? Style.colorPrimaryDark : Style.colorPrimary

    property font font: Style.textTheme.headline5

    property alias labelRectangle: labelRectangle
    property alias controlRectangle: controlRectangle
    property alias hintRectangle: hintRectangle

    property alias labelLabel: labelLabel
    property alias hintLabel: hintLabel

    Rectangle {
        id: labelRectangle
        anchors.left: root.left
        visible: root.haveALabel
        implicitHeight: root.cellHeight
        implicitWidth: root.haveALabel ? labelLabel.width+Style.formInnerMargin*2 : 0
        color: root.showLabelRectangle ? root.colorBackground : Style.colorTransparent
        border.color: root.showLabelRectangle ? root.borderColorDefault : Style.colorTransparent
        border.width: root.borderWidth

        topLeftRadius: root.borderRadius
        bottomLeftRadius: root.haveAHint ? 0 : root.borderRadius
        topRightRadius: 0
        bottomRightRadius: 0

        BasicLabel {
            id: labelLabel
            anchors.verticalCenter: parent.top
            anchors.verticalCenterOffset: root.defaultCellHeight/2
            anchors.left: parent.left
            anchors.leftMargin: Style.formInnerMargin
            width: root.fitLabel ? contentWidth : root.labelWidth-Style.formInnerMargin*2

            relativeBackgroundColor: root.colorBackground
            font: root.font
            text: root.label

            verticalAlignment: Text.AlignVCenter
            elide: root.fitLabel ? Text.ElideNone : Text.ElideRight
            maximumLineCount: 1
        }
    }

    Rectangle {
        id: hintRectangle
        visible: root.haveAHint
        anchors.left: labelRectangle.left
        anchors.right: controlRectangle.right
        anchors.top: controlRectangle.bottom
        implicitHeight: root.haveAHint ? root.infosHeight : 0
        color: root.showLabelRectangle ? ((root.warning && root.haveAnErrorMessage) ? root.colorWarning : root.borderColorDefault) : Style.colorTransparent

        topLeftRadius: 0
        bottomLeftRadius: root.borderRadius
        topRightRadius: 0
        bottomRightRadius: root.borderRadius

        BasicLabel {
            id: hintLabel
            anchors.fill: parent
            anchors.margins: Style.formInnerMargin

            relativeBackgroundColor: hintRectangle.color
            font: Style.textTheme.body1
            text: root.hint

            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            maximumLineCount: 1
        }
    }

    Rectangle {
        id: controlRectangle
        anchors.left: labelRectangle.right
        anchors.leftMargin: root.haveALabel ? -root.borderWidth : 0
        anchors.right: root.right
        implicitHeight: root.cellHeight
        color: root.colorControlBackground
        border.color: root.highlighted ? root.colorHighlight :
                      root.warning ? root.colorWarning : root.borderColorDefault
        border.width: root.borderWidth

        topLeftRadius: (root.haveALabel && root.showLabelRectangle) ? 0 : root.borderRadius
        bottomLeftRadius: (root.haveAHint || (root.haveALabel && root.showLabelRectangle)) ? 0 : root.borderRadius
        topRightRadius: root.borderRadius
        bottomRightRadius: root.haveAHint ? 0 : root.borderRadius
    }

    MouseArea {
        visible: !root.editable
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
