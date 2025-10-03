import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import Eco.Tier3.Database

FormObjectModel {
    id: root

    FormComboBoxDelegate {
        id: dbType
        label: qsTr("Type")
        options: StandardObjectModel {
            StandardObject {text: "MySQL"; value: SqlDatabaseTypes.MySQL}
            StandardObject {text: "SQLite"; value: SqlDatabaseTypes.SQLite}
        }
        textRole: "text"
        valueRole: "value"
        targetProperty: "dbType"
    }
    FormTextFieldDelegate {
        label: qsTr("Nom")
        validator: StringValidator {
            minSize: 1
        }
        targetProperty: "dbName"
    }
    FormTextFieldPathDelegate {
        visible: dbType.currentValue===SqlDatabaseTypes.SQLite
        label: qsTr("Chemin")
        validator: StringValidator {
            minSize: 1
        }
        targetProperty: "dbPath"
    }
    FormTextFieldDelegate {
        visible: dbType.currentValue===SqlDatabaseTypes.MySQL
        label: qsTr("Serveur")
        validator: Ipv4Validator {}
        targetProperty: "dbServer"
    }
    FormTextFieldDelegate {
        visible: dbType.currentValue===SqlDatabaseTypes.MySQL
        label: qsTr("Nom utilisateur")
        targetProperty: "dbUserName"
    }
    FormTextFieldPasswordDelegate {
        visible: dbType.currentValue===SqlDatabaseTypes.MySQL
        label: qsTr("Mot de passe")
        targetProperty: "dbPassword"
    }
    FormTextFieldDelegate {
        visible: dbType.currentValue===SqlDatabaseTypes.MySQL
        label: qsTr("Port")
        validator: SocketPortValidator{}
        targetProperty: "dbPort"
    }
}
