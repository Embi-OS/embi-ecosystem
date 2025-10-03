import QtQuick
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Styles

import Eco.Tier3.Axion

KeyboardStyle {
    id: root

    readonly property bool compactSelectionList: [InputEngine.InputMode.Pinyin, InputEngine.InputMode.Cangjie, InputEngine.InputMode.Zhuyin].indexOf(InputContext.inputEngine.inputMode) !== -1
    readonly property string fontFamily: Style.primaryFont
    readonly property double keyBackgroundMargin: Math.round(13 * scaleHint)
    readonly property double keyContentMargin: Math.round(45 * scaleHint)
    readonly property double keyRadius: Style.sharp ? 4 : 6
    readonly property double keyIconScale: scaleHint * 0.45
    readonly property string resourcePrefix: "qrc:/qt-project.org/imports/QtQuick/VirtualKeyboard/Styles/axion/"

    readonly property string inputLocale: InputContext.locale
    property color inputLocaleIndicatorColor: Style.colorWhite
    property Timer inputLocaleIndicatorHighlightTimer: Timer {
        interval: 1000
        onTriggered: root.inputLocaleIndicatorColor = "gray"
    }
    onInputLocaleChanged: {
        inputLocaleIndicatorColor = Style.colorWhite
        inputLocaleIndicatorHighlightTimer.restart()
    }

    keyboardDesignWidth: 2560
    keyboardDesignHeight: 640
    keyboardRelativeLeftMargin: 130 / keyboardDesignWidth
    keyboardRelativeRightMargin: 130 / keyboardDesignWidth
    keyboardRelativeTopMargin: 30 / keyboardDesignHeight
    keyboardRelativeBottomMargin: 30 / keyboardDesignHeight

    keyboardBackground: Rectangle {
        color: Style.colorPrimaryDarkest
        anchors.fill: parent
        radius: 0

        layer.enabled: true
        layer.effect: BasicElevationEffect {
            elevation: Style.dialogElevation
        }
    }

    keyPanel: KeyPanel {
        id: keyPanel
        Rectangle {
            id: keyBackground
            radius: root.keyRadius
            color: Style.colorPrimaryLight
            anchors.fill: keyPanel
            anchors.margins: root.keyBackgroundMargin
            Text {
                id: keySmallText
                text: control.smallText
                visible: control.smallTextVisible
                color: Style.colorWhiteFade
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: root.keyRadius*2
                font {
                    family: root.fontFamily
                    weight: Font.Normal
                    pixelSize: 38 * root.scaleHint
                    capitalization: control.uppercased ? Font.AllUppercase : Font.MixedCase
                }
            }
            Text {
                id: keyText
                text: control.displayText
                color: Style.colorWhite
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.centerIn: parent
                font {
                    family: root.fontFamily
                    weight: Font.Normal
                    pixelSize: 52 * root.scaleHint
                    capitalization: control.uppercased ? Font.AllUppercase : Font.MixedCase
                }
            }
        }
        states: [
            State {
                name: "pressed"
                when: control.pressed
                PropertyChanges {
                    target: keyBackground
                    opacity: 0.75
                }
                PropertyChanges {
                    target: keyText
                    opacity: 0.5
                }
            },
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: keyBackground
                    opacity: 0.75
                }
                PropertyChanges {
                    target: keyText
                    opacity: 0.05
                }
            }
        ]
    }

    backspaceKeyPanel: KeyPanel {
        id: backspaceKeyPanel
        Rectangle {
            id: backspaceKeyBackground
            radius: root.keyRadius
            color: Style.colorPrimary
            anchors.fill: backspaceKeyPanel
            anchors.margins: root.keyBackgroundMargin
            SvgColorImage {
                id: backspaceKeyIcon
                anchors.centerIn: parent
                icon: root.resourcePrefix + "images/backspace.svg"
                iconWidth: 159 * root.keyIconScale
                iconHeight: 88 * root.keyIconScale
                color: Style.colorWhite
            }
        }
        states: [
            State {
                name: "pressed"
                when: control.pressed
                PropertyChanges {
                    target: backspaceKeyBackground
                    opacity: 0.80
                }
                PropertyChanges {
                    target: backspaceKeyIcon
                    opacity: 0.6
                }
            },
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: backspaceKeyBackground
                    opacity: 0.8
                }
                PropertyChanges {
                    target: backspaceKeyIcon
                    opacity: 0.2
                }
            }
        ]
    }

    languageKeyPanel: KeyPanel {
        id: languageKeyPanel
        Rectangle {
            id: languageKeyBackground
            radius: root.keyRadius
            color: Style.colorPrimary
            anchors.fill: languageKeyPanel
            anchors.margins: root.keyBackgroundMargin
            SvgColorImage {
                id: languageKeyIcon
                anchors.centerIn: parent
                icon: root.resourcePrefix + "images/globe.svg"
                iconWidth: 144 * root.keyIconScale
                iconHeight: 144 * root.keyIconScale
                color: Style.colorWhite
            }
        }
        states: [
            State {
                name: "pressed"
                when: control.pressed
                PropertyChanges {
                    target: languageKeyBackground
                    opacity: 0.80
                }
                PropertyChanges {
                    target: languageKeyIcon
                    opacity: 0.75
                }
            },
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: languageKeyBackground
                    opacity: 0.8
                }
                PropertyChanges {
                    target: languageKeyIcon
                    opacity: 0.2
                }
            }
        ]
    }

    enterKeyPanel: KeyPanel {
        id: enterKeyPanel
        Rectangle {
            id: enterKeyBackground
            radius: root.keyRadius
            color: Style.colorPrimary
            anchors.fill: enterKeyPanel
            anchors.margins: root.keyBackgroundMargin
            SvgColorImage {
                id: enterKeyIcon
                anchors.centerIn: parent
                icon: {
                    switch (control.actionId) {
                    case EnterKeyAction.Go:
                    case EnterKeyAction.Send:
                    case EnterKeyAction.Next:
                    case EnterKeyAction.Done:
                        return root.resourcePrefix + "images/check.svg"
                    case EnterKeyAction.Search:
                        return root.resourcePrefix + "images/search.svg"
                    default:
                        return root.resourcePrefix + "images/enter.svg"
                    }
                }
                readonly property size enterKeyIconSize: {
                    switch (control.actionId) {
                    case EnterKeyAction.Go:
                    case EnterKeyAction.Send:
                    case EnterKeyAction.Next:
                    case EnterKeyAction.Done:
                        return Qt.size(170, 119)
                    case EnterKeyAction.Search:
                        return Qt.size(148, 148)
                    default:
                        return Qt.size(211, 80)
                    }
                }
                iconWidth: enterKeyIconSize.width * root.keyIconScale
                iconHeight: enterKeyIconSize.height * root.keyIconScale
                color: Style.colorWhite
            }
            Text {
                id: enterKeyText
                visible: text.length !== 0
                text: control.actionId !== EnterKeyAction.None ? control.displayText : ""
                clip: true
                fontSizeMode: Text.HorizontalFit
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: Style.colorAccent
                font {
                    family: root.fontFamily
                    weight: Font.Normal
                    pixelSize: 44 * root.scaleHint
                    capitalization: Font.AllUppercase
                }
                anchors.fill: parent
                anchors.margins: Math.round(42 * root.scaleHint)
            }
        }
        states: [
            State {
                name: "pressed"
                when: control.pressed
                PropertyChanges {
                    target: enterKeyBackground
                    opacity: 0.80
                }
                PropertyChanges {
                    target: enterKeyIcon
                    opacity: 0.6
                }
                PropertyChanges {
                    target: enterKeyText
                    opacity: 0.6
                }
            },
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: enterKeyBackground
                    opacity: 0.8
                }
                PropertyChanges {
                    target: enterKeyIcon
                    opacity: 0.2
                }
                PropertyChanges {
                    target: enterKeyText
                    opacity: 0.2
                }
            }
        ]
    }

    hideKeyPanel: KeyPanel {
        id: hideKeyPanel
        Rectangle {
            id: hideKeyBackground
            radius: root.keyRadius
            color: Style.colorPrimary
            anchors.fill: hideKeyPanel
            anchors.margins: root.keyBackgroundMargin
            SvgColorImage {
                id: hideKeyIcon
                anchors.centerIn: parent
                icon: root.resourcePrefix + "images/hidekeyboard.svg"
                iconWidth: 144 * root.keyIconScale
                iconHeight: 127 * root.keyIconScale
                color: Style.colorWhite
            }
        }
        states: [
            State {
                name: "pressed"
                when: control.pressed
                PropertyChanges {
                    target: hideKeyBackground
                    opacity: 0.80
                }
                PropertyChanges {
                    target: hideKeyIcon
                    opacity: 0.6
                }
            },
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: hideKeyBackground
                    opacity: 0.8
                }
                PropertyChanges {
                    target: hideKeyIcon
                    opacity: 0.2
                }
            }
        ]
    }

    shiftKeyPanel: KeyPanel {
        id: shiftKeyPanel
        Rectangle {
            id: shiftKeyBackground
            radius: root.keyRadius
            color: Style.colorPrimary
            anchors.fill: shiftKeyPanel
            anchors.margins: root.keyBackgroundMargin
            SvgColorImage {
                id: shiftKeyIcon
                anchors.centerIn: parent
                icon: root.resourcePrefix + "images/shift.svg"
                iconWidth: 144 * root.keyIconScale
                iconHeight: 134 * root.keyIconScale
                color: Style.colorWhite
            }
            states: [
                State {
                    name: "capsLockActive"
                    when: InputContext.capsLockActive
                    PropertyChanges {
                        target: shiftKeyBackground
                        color: Style.colorAccent
                    }
                    PropertyChanges {
                        target: shiftKeyIcon
                        color: Style.colorPrimary
                    }
                },
                State {
                    name: "shiftActive"
                    when: InputContext.shiftActive
                    PropertyChanges {
                        target: shiftKeyIcon
                        color: Style.colorAccent
                    }
                }
            ]
        }
        states: [
            State {
                name: "pressed"
                when: control.pressed
                PropertyChanges {
                    target: shiftKeyBackground
                    opacity: 0.80
                }
                PropertyChanges {
                    target: shiftKeyIcon
                    opacity: 0.6
                }
            },
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: shiftKeyBackground
                    opacity: 0.8
                }
                PropertyChanges {
                    target: shiftKeyIcon
                    opacity: 0.2
                }
            }
        ]
    }

    spaceKeyPanel: KeyPanel {
        id: spaceKeyPanel
        Rectangle {
            id: spaceKeyBackground
            radius: root.keyRadius
            color: Style.colorPrimary
            anchors.fill: spaceKeyPanel
            anchors.margins: root.keyBackgroundMargin
            Text {
                id: spaceKeyText
                text: "＿＿"
                color: root.inputLocaleIndicatorColor
                Behavior on color { PropertyAnimation { duration: 250 } }
                anchors.centerIn: parent
                font {
                    family: root.fontFamily
                    weight: Font.Normal
                    pixelSize: 52 * root.scaleHint
                }
            }
        }
        states: [
            State {
                name: "pressed"
                when: control.pressed
                PropertyChanges {
                    target: spaceKeyBackground
                    opacity: 0.80
                }
            },
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: spaceKeyBackground
                    opacity: 0.8
                }
            }
        ]
    }

    symbolKeyPanel: KeyPanel {
        id: symbolKeyPanel
        Rectangle {
            id: symbolKeyBackground
            radius: root.keyRadius
            color: Style.colorPrimary
            anchors.fill: symbolKeyPanel
            anchors.margins: root.keyBackgroundMargin
            Text {
                id: symbolKeyText
                text: control.displayText
                color: Style.colorWhite
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.centerIn: parent
                font {
                    family: root.fontFamily
                    weight: Font.Normal
                    pixelSize: 44 * root.scaleHint
                    capitalization: Font.AllUppercase
                }
            }
        }
        states: [
            State {
                name: "pressed"
                when: control.pressed
                PropertyChanges {
                    target: symbolKeyBackground
                    opacity: 0.80
                }
                PropertyChanges {
                    target: symbolKeyText
                    opacity: 0.6
                }
            },
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: symbolKeyBackground
                    opacity: 0.8
                }
                PropertyChanges {
                    target: symbolKeyText
                    opacity: 0.2
                }
            }
        ]
    }

    modeKeyPanel: KeyPanel {
        id: modeKeyPanel
        Rectangle {
            id: modeKeyBackground
            radius: root.keyRadius
            color: Style.colorPrimary
            anchors.fill: modeKeyPanel
            anchors.margins: root.keyBackgroundMargin
            Text {
                id: modeKeyText
                text: control.displayText
                color: Style.colorWhite
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.centerIn: parent
                font {
                    family: root.fontFamily
                    weight: Font.Normal
                    pixelSize: 44 * root.scaleHint
                    capitalization: Font.AllUppercase
                }
            }
            Rectangle {
                id: modeKeyIndicator
                implicitHeight: parent.height * 0.1
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.leftMargin: parent.width * 0.4
                anchors.rightMargin: parent.width * 0.4
                anchors.bottomMargin: parent.height * 0.12
                color: Style.colorAccent
                radius: root.keyRadius
                visible: control.mode
            }
        }
        states: [
            State {
                name: "pressed"
                when: control.pressed
                PropertyChanges {
                    target: modeKeyBackground
                    opacity: 0.80
                }
                PropertyChanges {
                    target: modeKeyText
                    opacity: 0.6
                }
            },
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: modeKeyBackground
                    opacity: 0.8
                }
                PropertyChanges {
                    target: modeKeyText
                    opacity: 0.2
                }
            }
        ]
    }

    handwritingKeyPanel: KeyPanel {
        id: handwritingKeyPanel
        Rectangle {
            id: hwrKeyBackground
            radius: root.keyRadius
            color: Style.colorPrimary
            anchors.fill: handwritingKeyPanel
            anchors.margins: root.keyBackgroundMargin
            SvgColorImage {
                id: hwrKeyIcon
                anchors.centerIn: parent
                icon: root.resourcePrefix + (keyboard.handwritingMode ? "images/textmode.svg" : "images/handwriting.svg")
                readonly property size hwrKeyIconSize: keyboard.handwritingMode ? Qt.size(124, 96) : Qt.size(156, 104)
                iconWidth: hwrKeyIconSize.width * root.keyIconScale
                iconHeight: hwrKeyIconSize.height * root.keyIconScale
                color: Style.colorWhite
            }
        }
        states: [
            State {
                name: "pressed"
                when: control.pressed
                PropertyChanges {
                    target: hwrKeyBackground
                    opacity: 0.80
                }
                PropertyChanges {
                    target: hwrKeyIcon
                    opacity: 0.6
                }
            },
            State {
                name: "disabled"
                when: !control.enabled
                PropertyChanges {
                    target: hwrKeyBackground
                    opacity: 0.8
                }
                PropertyChanges {
                    target: hwrKeyIcon
                    opacity: 0.2
                }
            }
        ]
    }

    characterPreviewMargin: 0
    characterPreviewDelegate: Item {
        property string text
        id: characterPreview
        Rectangle {
            id: characterPreviewBackground
            anchors.fill: parent
            color: "#5d5b59"
            radius: root.keyRadius
            Text {
                id: characterPreviewText
                color: Style.colorWhite
                text: characterPreview.text
                fontSizeMode: Text.HorizontalFit
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.fill: parent
                anchors.margins: Math.round(48 * root.scaleHint)
                font {
                    family: root.fontFamily
                    weight: Font.Normal
                    pixelSize: 82 * root.scaleHint
                }
            }
        }
    }

    alternateKeysListItemWidth: 99 * root.scaleHint
    alternateKeysListItemHeight: 150 * root.scaleHint
    alternateKeysListDelegate: Item {
        id: alternateKeysListItem
        width: root.alternateKeysListItemWidth
        height: root.alternateKeysListItemHeight
        Text {
            id: listItemText
            text: model.text
            color: "#868482"
            font {
                family: root.fontFamily
                weight: Font.Normal
                pixelSize: 52 * root.scaleHint
            }
            anchors.centerIn: parent
        }
        states: State {
            name: "current"
            when: alternateKeysListItem.ListView.isCurrentItem
            PropertyChanges {
                target: listItemText
                color: Style.colorWhite
            }
        }
    }
    alternateKeysListHighlight: Rectangle {
        color: "#5d5b59"
        radius: root.keyRadius
    }
    alternateKeysListBackground: Rectangle {
        color: Style.colorPrimary
        radius: root.keyRadius
    }

    selectionListHeight: 85 * root.scaleHint
    selectionListDelegate: SelectionListItem {
        id: selectionListItem
        height: 85 * root.scaleHint//ListView.view.height
        width: Math.round(selectionListLabel.width + selectionListLabel.anchors.leftMargin * 2)
        Text {
            id: selectionListLabel
            anchors.left: parent.left
            anchors.leftMargin: Math.round((root.compactSelectionList ? 50 : 140) * root.scaleHint)
            anchors.verticalCenter: parent.verticalCenter
            text: decorateText(display, wordCompletionLength)
            color: Style.colorAccent
            font {
                family: root.fontFamily
                weight: Font.Normal
                pixelSize: 44 * root.scaleHint
            }
            function decorateText(text, wordCompletionLength) {
                if (wordCompletionLength > 0) {
                    return text.slice(0, -wordCompletionLength) + '<u>' + text.slice(-wordCompletionLength) + '</u>'
                }
                return text
            }
        }
        Rectangle {
            id: selectionListSeparator
            width: 4 * root.scaleHint
            height: 36 * root.scaleHint
            radius: root.keyRadius
            color: Style.colorPrimary
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.left
        }
        states: State {
            name: "current"
            when: selectionListItem.ListView.isCurrentItem
            PropertyChanges {
                target: selectionListLabel
                color: Style.colorWhite
            }
        }
    }
    selectionListBackground: Rectangle {
        color: Style.colorPrimary
        radius: Style.contentRectangleRadius
    }
    selectionListAdd: Transition {
        NumberAnimation { property: "y"; from: wordCandidateView.height; duration: 200 }
        NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 200 }
    }
    selectionListRemove: Transition {
        NumberAnimation { property: "y"; to: -wordCandidateView.height; duration: 200 }
        NumberAnimation { property: "opacity"; to: 0; duration: 200 }
    }

    navigationHighlight: Rectangle {
        color: "transparent"
        border.color: "yellow"
        border.width: 5
    }

    traceInputKeyPanelDelegate: TraceInputKeyPanel {
        id: traceInputKeyPanel
        traceMargins: root.keyBackgroundMargin
        Rectangle {
            id: traceInputKeyPanelBackground
            radius: root.keyRadius
            color: Style.colorPrimary
            anchors.fill: traceInputKeyPanel
            anchors.margins: root.keyBackgroundMargin
            Text {
                id: hwrInputModeIndicator
                visible: control.patternRecognitionMode === InputEngine.PatternRecognitionMode.Handwriting
                text: {
                    switch (InputContext.inputEngine.inputMode) {
                    case InputEngine.InputMode.Numeric:
                        if (["ar", "fa"].indexOf(InputContext.locale.substring(0, 2)) !== -1)
                            return "\u0660\u0661\u0662"
                        // Fallthrough
                    case InputEngine.InputMode.Dialable:
                        return "123"
                    case InputEngine.InputMode.Greek:
                        return "ΑΒΓ"
                    case InputEngine.InputMode.Cyrillic:
                        return "АБВ"
                    case InputEngine.InputMode.Arabic:
                        if (InputContext.locale.substring(0, 2) === "fa")
                            return "\u0627\u200C\u0628\u200C\u067E"
                        return "\u0623\u200C\u0628\u200C\u062C"
                    case InputEngine.InputMode.Hebrew:
                        return "\u05D0\u05D1\u05D2"
                    case InputEngine.InputMode.ChineseHandwriting:
                        return "中文"
                    case InputEngine.InputMode.JapaneseHandwriting:
                        return "日本語"
                    case InputEngine.InputMode.KoreanHandwriting:
                        return "한국어"
                    case InputEngine.InputMode.Thai:
                        return "กขค"
                    default:
                        return "Abc"
                    }
                }
                color: Style.colorWhite
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: root.keyContentMargin
                font {
                    family: root.fontFamily
                    weight: Font.Normal
                    pixelSize: 44 * root.scaleHint
                    capitalization: {
                        if (InputContext.capsLockActive)
                            return Font.AllUppercase
                        if (InputContext.shiftActive)
                            return Font.MixedCase
                        return Font.AllLowercase
                    }
                }
            }
        }
        Canvas {
            id: traceInputKeyGuideLines
            anchors.fill: traceInputKeyPanelBackground
            opacity: 0.1
            onPaint: {
                var ctx = getContext("2d")
                ctx.lineWidth = 1
                ctx.strokeStyle = Qt.rgba(0xFF, 0xFF, 0xFF)
                ctx.clearRect(0, 0, width, height)
                var i
                var margin = Math.round(30 * root.scaleHint)
                if (control.horizontalRulers) {
                    for (i = 0; i < control.horizontalRulers.length; i++) {
                        ctx.beginPath()
                        var y = Math.round(control.horizontalRulers[i])
                        var rightMargin = Math.round(width - margin)
                        if (i + 1 === control.horizontalRulers.length) {
                            ctx.moveTo(margin, y)
                            ctx.lineTo(rightMargin, y)
                        } else {
                            var dashLen = Math.round(20 * root.scaleHint)
                            for (var dash = margin, dashCount = 0;
                                 dash < rightMargin; dash += dashLen, dashCount++) {
                                if ((dashCount & 1) === 0) {
                                    ctx.moveTo(dash, y)
                                    ctx.lineTo(Math.min(dash + dashLen, rightMargin), y)
                                }
                            }
                        }
                        ctx.stroke()
                    }
                }
                if (control.verticalRulers) {
                    for (i = 0; i < control.verticalRulers.length; i++) {
                        ctx.beginPath()
                        ctx.moveTo(control.verticalRulers[i], margin)
                        ctx.lineTo(control.verticalRulers[i], Math.round(height - margin))
                        ctx.stroke()
                    }
                }
            }
            Connections {
                target: control

                function onHorizontalRulersChanged() {
                    traceInputKeyGuideLines.requestPaint();
                }
                function onVerticalRulersChanged() {
                    traceInputKeyGuideLines.requestPaint();
                }
            }
        }
    }

    traceCanvasDelegate: TraceCanvas {
        id: traceCanvas
        onAvailableChanged: {
            if (!available)
                return
            var ctx = getContext("2d")
            if (parent.canvasType === "fullscreen") {
                ctx.lineWidth = 10
                ctx.strokeStyle = Qt.rgba(0, 0, 0)
            } else {
                ctx.lineWidth = 10 * root.scaleHint
                ctx.strokeStyle = Qt.rgba(0xFF, 0xFF, 0xFF)
            }
            ctx.lineCap = "round"
            ctx.fillStyle = ctx.strokeStyle
        }
        autoDestroyDelay: 800
        onTraceChanged: if (trace === null) opacity = 0
        Behavior on opacity { PropertyAnimation { easing.type: Easing.OutCubic; duration: 150 } }
    }

    popupListDelegate: SelectionListItem {
        property double cursorAnchor: popupListLabel.x + popupListLabel.width
        id: popupListItem
        width: popupListLabel.width + popupListLabel.anchors.leftMargin * 2
        height: popupListLabel.height + popupListLabel.anchors.topMargin * 2
        Text {
            id: popupListLabel
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: popupListLabel.height / 2
            anchors.topMargin: popupListLabel.height / 3
            text: decorateText(display, wordCompletionLength)
            color: "#5CAA15"
            font {
                family: root.fontFamily
                weight: Font.Normal
                pixelSize: InputMethod.cursorRectangle.height * 0.8
            }
            function decorateText(text, wordCompletionLength) {
                if (wordCompletionLength > 0) {
                    return text.slice(0, -wordCompletionLength) + '<u>' + text.slice(-wordCompletionLength) + '</u>'
                }
                return text
            }
        }
        states: State {
            name: "current"
            when: popupListItem.ListView.isCurrentItem
            PropertyChanges {
                target: popupListLabel
                color: "black"
            }
        }
    }

    popupListBackground: Item {
        Rectangle {
            width: parent.width
            height: parent.height
            color: Style.colorWhite
            border {
                width: 1
                color: "#929495"
            }
        }
    }

    popupListAdd: Transition {
        NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 200 }
    }

    popupListRemove: Transition {
        NumberAnimation { property: "opacity"; to: 0; duration: 200 }
    }

    languagePopupListEnabled: true

    languageListDelegate: SelectionListItem {
        id: languageListItem
        width: languageNameTextMetrics.width * 17
        height: languageNameTextMetrics.height + languageListLabel.anchors.topMargin + languageListLabel.anchors.bottomMargin
        Text {
            id: languageListLabel
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: languageNameTextMetrics.height / 2
            anchors.rightMargin: anchors.leftMargin
            anchors.topMargin: languageNameTextMetrics.height / 3
            anchors.bottomMargin: anchors.topMargin
            text: languageNameFormatter.elidedText
            color: Style.colorAccent
            font {
                family: root.fontFamily
                weight: Font.Normal
                pixelSize: 44 * root.scaleHint
            }
        }
        TextMetrics {
            id: languageNameTextMetrics
            font {
                family: root.fontFamily
                weight: Font.Normal
                pixelSize: 44 * root.scaleHint
            }
            text: "X"
        }
        TextMetrics {
            id: languageNameFormatter
            font {
                family: root.fontFamily
                weight: Font.Normal
                pixelSize: 44 * root.scaleHint
            }
            elide: Text.ElideRight
            elideWidth: languageListItem.width - languageListLabel.anchors.leftMargin - languageListLabel.anchors.rightMargin
            text: displayName
        }
        states: State {
            name: "current"
            when: languageListItem.ListView.isCurrentItem
            PropertyChanges {
                target: languageListLabel
                color: "black"
            }
        }
    }

    languageListBackground: Rectangle {
        color: Style.colorWhite
        border {
            width: 1
            color: "#929495"
        }
    }

    languageListAdd: Transition {
        NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 200 }
    }

    languageListRemove: Transition {
        NumberAnimation { property: "opacity"; to: 0; duration: 200 }
    }

    selectionHandle: Image {
        sourceSize.width: 20
        source: root.resourcePrefix + "images/selectionhandle-bottom.svg"
        asynchronous: true
    }

    fullScreenInputContainerBackground: Rectangle {
        color: "#FFF"
    }

    fullScreenInputBackground: Rectangle {
        color: "#FFF"
    }

    fullScreenInputMargins: Math.round(15 * root.scaleHint)

    fullScreenInputPadding: Math.round(30 * root.scaleHint)

    fullScreenInputCursor: Rectangle {
        width: 1
        color: "#000"
        visible: parent.blinkStatus
    }

    fullScreenInputFont.pixelSize: 58 * root.scaleHint
}
