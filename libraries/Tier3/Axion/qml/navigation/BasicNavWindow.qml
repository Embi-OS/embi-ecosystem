import QtQuick
import Eco.Tier3.Axion

BasicNavItem {
    id: root

    property string titleText: ""
    property string instructionsText: ""

    property bool titleInstructionVisible: titleText!=="" || instructionsText!==""

    property int mainComponentTopMargin: Style.mainComponentTopMargin
    property int mainComponentBottomMargin: Style.mainComponentBottomMargin
    property int mainComponentLeftMargin: Style.mainComponentLeftMargin
    property int mainComponentRightMargin: Style.mainComponentRightMargin

    onItemIsActive: {
        if(stackItemName!=="")
            console.info("BasicNavWindow active:",stackItemName,stackItemSecondaryName!==""?"-> \""+stackItemSecondaryName+"\"":"")
    }

    onQuitConfirmed: {
        root.state="";
    }

//──────────────────────────────────────────────────────────────────────
// Déclaration des composants globaux à toutes les pages
//──────────────────────────────────────────────────────────────────────

    LabelWithCaption {
        anchors.top: parent.top
        anchors.topMargin: Style.titleTopMarginDefault
        anchors.horizontalCenter: parent.horizontalCenter

        visible: root.titleInstructionVisible

        haveAText: !(text === "") || haveACaption
        text: root.titleText
        textFont: Style.textTheme.title1
        caption: root.instructionsText
        captionFont: Style.textTheme.subtitle1
        spacing: Style.instructionsSpaceDefault
        horizontalAlignment: Text.AlignHCenter
    }
}
