pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T
import Eco.Tier1.Utils
import Eco.Tier3.Axion

T.Popup {
    id: root

    signal imageClicked()
    property alias screenCapture: screenCapture

    readonly property Window rootWindow: parent.Window.window as Window

    x: Math.round((parent.width - width*1.1))
    y: Math.round((parent.height - height*1.1))
    height: parent.height/2.5
    width: parent.width/2.5

    modal: true // Back darker
    focus: true
    closePolicy: T.Popup.CloseOnPressOutside

    onClosed: captureImage.source=""

    enter: Transition {
      NumberAnimation { property: "scale";from: 0.9;to: 1.0;easing.type: Easing.OutQuint;duration: 220 }
      NumberAnimation { property: "opacity";from: 0.0;to: 1.0;easing.type: Easing.OutCubic;duration: 150 }
    }

    exit: Transition {
      NumberAnimation { property: "scale";from: 1.0;to: 0.9;easing.type: Easing.OutQuint;duration: 220 }
      NumberAnimation { property: "opacity";from: 1.0;to: 0.0;easing.type: Easing.OutCubic;duration: 150 }
    }

    T.Overlay.modal: BasicOverlay {}
    T.Overlay.modeless: BasicOverlay {}

    property double elevation: Style.dialogElevation
    property color backgroundColor: Style.colorPrimaryDark

    background: Rectangle {
        color: root.backgroundColor

        layer.enabled: root.elevation>0
        layer.effect: BasicElevationEffect {
            elevation: root.elevation
        }
    }

    contentItem: Image {
        id: captureImage
        asynchronous: true
        fillMode: Image.PreserveAspectFit

        TapHandler {
            onTapped: root.imageClicked();
        }
    }

    KeyEventNotifier {
        id: keyEventNotifier
        enabled: root.enabled
        window: root.rootWindow
        onKeyPressed: (key) => {
            if(key===Qt.Key_Print)
                screenCapture.capture("",root.rootWindow.contentItem)
        }
    }

    ItemCapturer {
        id: screenCapture
        onCaptureFinished: (result, path) => {
            if(!result)
                return
            captureImage.source="file://"+path;
            root.visible=true;
            JS.singleshotTimer(1000, function(){
                root.visible=false;
            });
        }
    }
}
