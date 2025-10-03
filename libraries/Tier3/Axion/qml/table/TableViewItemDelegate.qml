pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion
import Qt.labs.qmlmodels

DelegateChooser {
    id: root
    role: "column_delegate"

    DelegateChoice {
        roleValue: TableViewColumnDelegates.Dot
        BasicTableViewDelegate {
            id: dotDelegate
            required property bool display
            required property color columnColor
            readonly property bool isRowCurrent: TableView.view.currentRow===row

            current: isRowCurrent
            foregroundColor: display ? columnColor : Style.colorWhite

            contentItem: Item {
                Rectangle {
                    anchors.centerIn: parent
                    height: Math.min(parent.height, parent.width)/1.5
                    width: height
                    radius: height/2
                    color: dotDelegate.foregroundColor
                }
            }
        }
    }

    DelegateChoice {
        roleValue: TableViewColumnDelegates.Color
        BasicTableViewDelegate {
            required property color display
            required property font column_font
            readonly property bool isRowCurrent: TableView.view.currentRow===row

            current: isRowCurrent
            backgroundColor: display
            text: ColorUtils.name(display, false)
            font: column_font
            foregroundColor: ColorUtils.transparent(ColorUtils.isDarkColor(backgroundColor) ? Style.colorWhite : Style.colorBlack, 0.6)
        }
    }

    DelegateChoice {
        BasicTableViewDelegate {
            required property string display
            required property color columnColor
            required property font column_font
            readonly property bool isRowCurrent: TableView.view.currentRow===row

            current: isRowCurrent
            text: display
            font: column_font
            foregroundColor: columnColor
        }
    }
}
