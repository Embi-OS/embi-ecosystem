import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

Item {
    id: root

    property string stackItemName: ""
    property string stackItemSecondaryName: ""
    property StandardObjectModel navModels: StandardObjectModel{}
    property StandardObjectModel debugModel: StandardObjectModel{}

    readonly property BasicStackView stack: StackView.view as BasicStackView
    readonly property int stackStatus: StackView.status
    readonly property int stackIndex: StackView.index
    property int stackIndexTmp: -1
    readonly property bool isStackActive: stackStatus === StackView.Active

    property string quitButtonType
    property var quitButtonValue

    signal itemIsReady()
    signal itemIsActive()
    signal itemIsDestroyed()

    signal itemActivated()
    signal itemActivating()
    signal itemDeactivated()
    signal itemDeactivating()
    signal itemRemoved()

    StackView.onActivated: root.itemActivated()
    StackView.onActivating: root.itemActivating()
    StackView.onDeactivated: root.itemDeactivated()
    StackView.onDeactivating: root.itemDeactivating()
    StackView.onRemoved: root.itemRemoved()

    signal aboutToQuit(string buType, var buValue)
    signal quitConfirmed(string buType, var buValue)
    signal navButtonClicked(int barIndex, int naIndex)
    signal cardButtonClicked(var caValue)
    signal mainButtonClicked(var maValue)
    signal extraButtonClicked(var exValue)
    signal backButtonClicked()

    property bool navigationLocked: false
    property bool navigationEnabled: true

    property bool isCompleted: false
    property bool isReady: false
    Component.onCompleted: {
        root.isCompleted=true
    }

    Component.onDestruction: {
        root.isReady=false
        root.isCompleted=false
    }

    onItemIsReady: {
        console.info("BasicNavItem active:",stackItemName,stackItemSecondaryName!==""?"-> \""+stackItemSecondaryName+"\"":"")
        root.isReady=true
    }

    onStackStatusChanged: {
        switch (stackStatus){
        case StackView.Inactive:
            break;
        case StackView.Deactivating:
            break;
        case StackView.Activating:
            break;
        case StackView.Active:
            Qt.callLater(itemIsActive);
            if(stackIndex !== stackIndexTmp) {
                stackIndexTmp = stackIndex;
                Qt.callLater(itemIsReady);
            }
            break;
        }
    }

    onStackIndexChanged: {
        if(stackIndex>=0) {

        }
        else {
            stackIndexTmp = -1;
            itemIsDestroyed();
        }
    }

    function showQuitDialog() {
        var settings = {
            "onAccepted": function() {
                root.confirmQuit(quitButtonType, quitButtonValue);
            }
        }
        DialogManager.showQuit(settings);
    }

    function confirmQuit (buType: string, buValue: var) {
        root.aboutToQuit(buType, buValue)
        if(buType==="back" && root.stack) {
            root.stack.popLast()
        }
        else {
            quitConfirmed(buType, buValue)
        }
    }

    function wantToQuit() {
        Qt.callLater(root.wantToQuitHandler)
    }

    function wantToQuitHandler() {
        if(!root.isStackActive)
            return;

        if(root.navigationLocked) {
            DialogManager.showWarning(qsTr("Impossible de quitter la page maintenant!"));
        }
        else if(!root.navigationEnabled) {
            showQuitDialog();
        }
        else {
            root.confirmQuit(quitButtonType, quitButtonValue);
        }
    }

    onNavButtonClicked: (barIndex, naIndex) => {
        quitButtonType = `nav.${barIndex}.${naIndex}`;
        quitButtonValue = naIndex;
        wantToQuit();
    }

    onCardButtonClicked: (caValue) => {
        quitButtonType = `card.${caValue}`;
        quitButtonValue = caValue;
        wantToQuit();
    }

    onMainButtonClicked: (maValue) => {
        quitButtonType = "main";
        quitButtonValue = maValue;
        wantToQuit();
    }

    onExtraButtonClicked: (exValue) => {
        quitButtonType = "extra";
        quitButtonValue = exValue;
        wantToQuit();
    }

    onBackButtonClicked: {
        quitButtonType = "back";
        quitButtonValue = 0;
        wantToQuit();
    }
}
