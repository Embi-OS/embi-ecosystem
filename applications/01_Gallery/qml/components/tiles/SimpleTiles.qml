import QtQuick
import Eco.Tier3.Axion

RowLayout {
    id: root

    implicitWidth: Math.min(640, parent.width)
    implicitHeight: Math.min(300, parent.height)


    property bool concealed: checkBox.checked

    ColumnLayout {
        Layout.fillHeight: true

        BasicCheckBox {
            id: checkBox
            text: "Concealed"
        }

        BasicTile {
            id: tile0
            Layout.fillHeight: true
            Layout.preferredWidth: height
            concealed: root.concealed
            text: "COUPLE MAX" + "\n" +
                  ("%1 x %2").arg(width).arg(height)
        }

        TileClickable {
            id: tile1
            Layout.fillHeight: true
            Layout.preferredWidth: height
            icon.color: checked ? Style.colorFatal : Style.colorSuccess
            checkable: true
            height: 500
            text: qsTr("Sens d'utilisation")
            value: checked ? qsTr("Vissage") : qsTr("Dévissage")
            icon.source: checked ? MaterialIcons.rotateRight : MaterialIcons.rotateLeft
        }

        TileSpinbox {
            id: tile2
            Layout.fillHeight: true
            Layout.preferredWidth: height
            text: qsTr("Nb. crans par tours")
            from: 1
            to: 60
            value: 6
        }
    }

    ColumnLayout {
        Layout.fillHeight: true
        TileValue {
            id: tile3
            Layout.fillHeight: true
            Layout.preferredWidth: height
            concealed: root.concealed
            text: qsTr("COUPLE")
            valueUnit: "mNm"
            valueColor: value<5 ? Style.colorFatal :
                        value<10 ? Style.colorError :
                        value<15 ? Style.colorWarning : Style.colorSuccess

            SequentialAnimation on value {
                running: true
                loops: NumberAnimation.Infinite
                NumberAnimation { from: 0; to: 20; duration: 5000 }
                NumberAnimation { from: 20; to: 0; duration: 5000 }
            }
        }
        TileCircularGauge {
            id: tile4
            Layout.fillHeight: true
            Layout.preferredWidth: height
            concealed: root.concealed
            text: qsTr("COUPLE")
            valueUnit: "mNm"
            value: tile3.value
            tolMin: 5.0
            tolMax: 15.0

            showMarble: true
            marbleValue: 10.0
        }
        TileMiniChart {
            id: tile5
            Layout.fillHeight: true
            Layout.preferredWidth: height
            concealed: root.concealed
        }
    }

    TileLinearGauge {
        Layout.fillHeight: true
        concealed: root.concealed
        text: qsTr("DEPLACEMENT")
        valueUnit: "mm"
        value: tile3.value
        tolMin: 6.0
        tolMax: 14.0

        showMarble: true
        marbleValue: 10.0
    }
}
