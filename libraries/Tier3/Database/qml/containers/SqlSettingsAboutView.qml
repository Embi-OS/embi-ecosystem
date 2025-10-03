import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.Database

PaneTreeView {
    id: root

    signal editButtonClicked()
    signal vacuumButtonClicked()
    signal exportButtonClicked()
    signal importButtonClicked()
    signal dropButtonClicked()

    title: qsTr("Réglages de la connexion à la DB globale")

    model: proxyModel

    ProxyModel {
        id: proxyModel
        delayed: true
        sourceModel: treeModel
        filterRoleName: "visible"
        filterValue: true
    }

    onEditButtonClicked: {
        var component = Qt.createComponent("Eco.Tier3.Database", "Form_SettingsSql");
        var model = component.createObject(root) as FormObjectModel
        var settings = {
            "title": qsTr("Paramètres Database"),
            "formModel": model,
            "buttonHelp": qsTr("Tester"),
            "formObject": SqlManager,
            "onHelpRequested": function() {
                var tempParams = model.currentValues();
                SqlManager.testConnexion(tempParams)
            },
            "onClosed": function () {
                model.destroy()
            },
            "onFormValidated": function(formValues) {
                AxionHelper.warningRestart()
            }
        }

        DialogManager.showForm(settings);
    }

    onVacuumButtonClicked: {
        var settings = {
            "message": qsTr("Êtes-vous sûr de vouloir optimiser la base de données ?"),
            "infos": qsTr("Le processus ne pourra pas être arrété"),
            "buttonAccept": qsTr("Optimiser"),
            "buttonReject": qsTr("Annuler"),
            "onAccepted": function() {
                SqlManager.vacuumDatabase();
            }
        }
        DialogManager.showMessage(settings);
    }

    onExportButtonClicked: {
        var settings = {
            "message": qsTr("Ou voullez-vous exporter la base de données ?"),
            "selectionType": FolderTreeTypes.Dir,
            "buttonAccept": qsTr("Exporter"),
            "buttonReject": qsTr("Annuler"),
            "onPathSelected": function(path) {
                SqlManager.exportDatabase(path)
            }
        }
        DialogManager.showFileTree(settings);
    }

    onImportButtonClicked: {
        var settings = {
            "message": qsTr("D'ou voullez-vous importer la base de données ?"),
            "selectionType": FolderTreeTypes.Dir,
            "buttonAccept": qsTr("Importer"),
            "buttonReject": qsTr("Annuler"),
            "onPathSelected": function(path) {
                SqlManager.importDatabase(path)
            }
        }
        DialogManager.showFileTree(settings);
    }

    onDropButtonClicked: {
        var settings = {
            "message": qsTr("Êtes-vous sûr de vouloir supprimer la base de données ?"),
            "infos": qsTr("Le processus ne pourra pas être arrété"),
            "trace": qsTr("L'application va ensuite redémarrer !"),
            "buttonAccept": qsTr("Supprimer"),
            "buttonReject": qsTr("Annuler"),
            "onAccepted": function() {
                SqlManager.dropDatabase();
            }
        }
        DialogManager.showMessage(settings);
    }

    StandardObjectModel {
        id: treeModel

        InfoTreeDelegate {text: "DB type"; info: SqlDatabaseTypes.asString(SqlManager.dbType) }
        InfoTreeDelegate {text: "DB name"; info: SqlManager.dbName }
        InfoTreeDelegate {text: "DB path"; info: SqlManager.dbPath;        visible: SqlManager.dbType===SqlDatabaseTypes.SQLite }
        InfoTreeDelegate {text: "DB server"; info: SqlManager.dbServer;    visible: SqlManager.dbType===SqlDatabaseTypes.MySQL }
        InfoTreeDelegate {text: "DB port"; info: SqlManager.dbPort;        visible: SqlManager.dbType===SqlDatabaseTypes.MySQL }
        InfoTreeDelegate {text: "DB user"; info: SqlManager.dbUserName;    visible: SqlManager.dbType===SqlDatabaseTypes.MySQL }

        FormButtonDelegate {
            visible: root.editable
            label: qsTr("Editer")
            icon: MaterialIcons.databaseEdit
            onClicked: root.editButtonClicked()
        }
        FormButtonDelegate {
            label: qsTr("Optimiser la base de données")
            icon: MaterialIcons.broom
            onClicked: root.vacuumButtonClicked()
        }

        SubtitleTreeDelegate {
            visible: SqlManager.dbType===SqlDatabaseTypes.SQLite
            text: qsTr("Transfert");

            FormButtonDelegate {
                label: qsTr("Exporter la base de données")
                icon: MaterialIcons.databaseExport
                onClicked: root.exportButtonClicked()
            }
            SeparatorTreeDelegate {}
            FormButtonDelegate {
                label: qsTr("Importer la base de données")
                icon: MaterialIcons.databaseImport
                onClicked: root.importButtonClicked()
            }
            FormButtonDelegate {
                label: qsTr("Supprimer la base de données")
                icon: MaterialIcons.databaseRemove
                onClicked: root.dropButtonClicked()
            }
        }
    }
}
