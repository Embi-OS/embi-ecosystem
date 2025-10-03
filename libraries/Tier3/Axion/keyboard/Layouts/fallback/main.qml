// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Components
import QtQuick.Layouts

KeyboardLayout {
    id: root
    inputMode: InputEngine.InputMode.Latin
    keyWeight: 160
    readonly property double normalKeyWidth: normalKey.width
    readonly property double functionKeyWidth: mapFromItem(normalKey, normalKey.width / 2, 0).x
    KeyboardRow {
        Key {
            key: Qt.Key_1
            text: "1"
            alternativeKeys: "1+"
            smallText: "+"
            smallTextVisible: true
        }
        Key {
            key: Qt.Key_2
            text: "2"
            alternativeKeys: "2\""
            smallText: "\""
            smallTextVisible: true
        }
        Key {
            key: Qt.Key_3
            text: "3"
            alternativeKeys: "3*"
            smallText: "*"
            smallTextVisible: true
        }
        Key {
            key: Qt.Key_4
            text: "4"
        }
        Key {
            key: Qt.Key_5
            text: "5"
            alternativeKeys: "5%"
            smallText: "%"
            smallTextVisible: true
        }
        Key {
            key: Qt.Key_6
            text: "6"
            alternativeKeys: "6&"
            smallText: "&"
            smallTextVisible: true
        }
        Key {
            key: Qt.Key_7
            text: "7"
            alternativeKeys: "7/"
            smallText: "/"
            smallTextVisible: true
        }
        Key {
            key: Qt.Key_8
            text: "8"
            alternativeKeys: "8("
            smallText: "("
            smallTextVisible: true
        }
        Key {
            key: Qt.Key_9
            text: "9"
            alternativeKeys: "9)"
            smallText: ")"
            smallTextVisible: true
        }
        Key {
            key: Qt.Key_0
            text: "0"
            alternativeKeys: "0="
            smallText: "="
            smallTextVisible: true
        }
    }
    KeyboardRow {
        Key {
            key: Qt.Key_Q
            text: "q"
        }
        Key {
            id: normalKey
            key: Qt.Key_W
            text: "w"
        }
        Key {
            key: Qt.Key_E
            text: "e"
            alternativeKeys: "éèeêë"
        }
        Key {
            key: Qt.Key_R
            text: "r"
        }
        Key {
            key: Qt.Key_T
            text: "t"
        }
        Key {
            key: Qt.Key_Z
            text: "z"
        }
        Key {
            key: Qt.Key_U
            text: "u"
            alternativeKeys: "ùuûü"
        }
        Key {
            key: Qt.Key_I
            text: "i"
            alternativeKeys: "îiï"
        }
        Key {
            key: Qt.Key_O
            text: "o"
            alternativeKeys: "ôoœ"
        }
        Key {
            key: Qt.Key_P
            text: "p"
        }
    }
    KeyboardRow {
        KeyboardRow {
            Layout.preferredWidth: root.functionKeyWidth
            Layout.fillWidth: false
            FillerKey {
            }
            Key {
                key: Qt.Key_A
                text: "a"
                alternativeKeys: "aàâæ"
                weight: root.normalKeyWidth
                Layout.fillWidth: false
            }
        }
        Key {
            key: Qt.Key_S
            text: "s"
        }
        Key {
            key: Qt.Key_D
            text: "d"
        }
        Key {
            key: Qt.Key_F
            text: "f"
        }
        Key {
            key: Qt.Key_G
            text: "g"
        }
        Key {
            key: Qt.Key_H
            text: "h"
        }
        Key {
            key: Qt.Key_J
            text: "j"
        }
        Key {
            key: Qt.Key_K
            text: "k"
        }
        KeyboardRow {
            Layout.preferredWidth: root.functionKeyWidth
            Layout.fillWidth: false
            Key {
                key: Qt.Key_L
                text: "l"
                weight: root.normalKeyWidth
                Layout.fillWidth: false
            }
            FillerKey {
            }
        }
    }
    KeyboardRow {
        ShiftKey {
            weight: root.functionKeyWidth
            Layout.fillWidth: false
        }
        Key {
            key: Qt.Key_Y
            text: "y"
            alternativeKeys: "yÿ"
        }
        Key {
            key: Qt.Key_X
            text: "x"
        }
        Key {
            key: Qt.Key_C
            text: "c"
            alternativeKeys: "cç"
        }
        Key {
            key: Qt.Key_V
            text: "v"
        }
        Key {
            key: Qt.Key_B
            text: "b"
        }
        Key {
            key: Qt.Key_N
            text: "n"
            alternativeKeys: "n-"
            smallText: "-"
            smallTextVisible: true
        }
        Key {
            key: Qt.Key_M
            text: "m"
            alternativeKeys: "m_"
            smallText: "_"
            smallTextVisible: true
        }
        BackspaceKey {
            weight: root.functionKeyWidth
            Layout.fillWidth: false
        }
    }
    KeyboardRow {
        SymbolModeKey {
            weight: root.functionKeyWidth
            Layout.fillWidth: false
        }
        ChangeLanguageKey {
            weight: root.normalKeyWidth
            Layout.fillWidth: false
        }
        Key {
            key: Qt.Key_Comma
            weight: root.normalKeyWidth
            Layout.fillWidth: false
            text: ","
            alternativeKeys: ",<>\\"
            smallText: "<>\\"
            smallTextVisible: true
            highlighted: true
        }
        SpaceKey {
        }
        Key {
            key: Qt.Key_Period
            weight: root.normalKeyWidth
            Layout.fillWidth: false
            text: "."
            alternativeKeys: "!.?;:"
            smallText: "!?;:"
            smallTextVisible: true
            highlighted: true
        }
        HideKeyboardKey {
            weight: root.normalKeyWidth
            Layout.fillWidth: false
        }
        EnterKey {
            weight: root.functionKeyWidth
            Layout.fillWidth: false
        }
    }
}
