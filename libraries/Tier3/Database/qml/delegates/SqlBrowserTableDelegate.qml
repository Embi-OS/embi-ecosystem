pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion
import Eco.Tier3.Database

import Qt.labs.qmlmodels 1.0

DelegateChooser {
    id: root
    role: "type"

    property bool editable: false

    DelegateChoice {
        roleValue: SqlBrowserDataTypes.Boolean
        BasicTableViewDelegate {
            id: boolDelegate
            required property bool dirty
            required property bool display
            backgroundBorderColor: dirty ? Style.colorWarning : backgroundColor
            backgroundBorderWidth: Style.contentRectangleBorderWidth/2

            contentItem: BasicCheckBox {
                enabled: root.editable
                checked: boolDelegate.display
                onCheckedChanged: boolDelegate.model.edit = checked
            }
        }
    }

    DelegateChoice {
        roleValue: SqlBrowserDataTypes.Bytes
        BasicTableViewDelegate {
            id: hexDelegate
            required property bool dirty
            required property var display
            readonly property bool highlighted: (contentItem as BasicTextField)?.activeFocus ?? false
            backgroundBorderColor: highlighted ? Style.colorAccent :
                                   dirty ? Style.colorWarning : backgroundColor
            backgroundBorderWidth: Style.contentRectangleBorderWidth/2

            contentItem: BasicTextField {
                verticalAlignment: Text.AlignVCenter
                clip: true
                color: acceptableInput ? Style.colorWhite : Style.colorWarning

                enabled: root.editable
                font: hexDelegate.font
                text: FormatUtils.hex(hexDelegate.display)
                onAccepted: hexDelegate.model.edit = FormatUtils.fromHex(text)
            }
        }
    }

    DelegateChoice {
        roleValue: SqlBrowserDataTypes.Number
        BasicTableViewDelegate {
            id: numberDelegate
            required property bool dirty
            required property string display
            required property string type
            readonly property bool highlighted: (contentItem as BasicTextField)?.activeFocus ?? false
            backgroundBorderColor: highlighted ? Style.colorAccent :
                                   dirty ? Style.colorWarning : backgroundColor
            backgroundBorderWidth: Style.contentRectangleBorderWidth/2

            contentItem: BasicTextField {
                verticalAlignment: Text.AlignVCenter
                clip: true
                color: acceptableInput ? Style.colorWhite : Style.colorWarning

                enabled: root.editable
                font: numberDelegate.font
                text: numberDelegate.display
                onAccepted: numberDelegate.model.edit = text

                inputMethodHints: Qt.ImhFormattedNumbersOnly
            }
        }
    }

    DelegateChoice {
        roleValue: SqlBrowserDataTypes.Text
        BasicTableViewDelegate {
            id: textDelegate
            required property bool dirty
            required property string display
            required property string type
            readonly property bool highlighted: (contentItem as BasicTextField)?.activeFocus ?? false
            backgroundBorderColor: highlighted ? Style.colorAccent :
                                   dirty ? Style.colorWarning : backgroundColor
            backgroundBorderWidth: Style.contentRectangleBorderWidth/2

            contentItem: BasicTextField {
                verticalAlignment: Text.AlignVCenter
                clip: true
                color: acceptableInput ? Style.colorWhite : Style.colorWarning

                enabled: root.editable
                font: textDelegate.font
                text: textDelegate.display
                onAccepted: textDelegate.model.edit = text
            }
        }
    }

    DelegateChoice {
        BasicTableViewDelegate {
            required property string display
            text: display
        }
    }
}
