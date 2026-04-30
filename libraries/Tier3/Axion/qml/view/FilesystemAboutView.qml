import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

PaneTreeView {
    id: root

    title: qsTr("Informations sur le système de fichier")

    model: treeModel

    StandardObjectModel {
        id: treeModel
        InfoTreeDelegate {text: "FS homePath";info: Filesystem.homePath}
        InfoTreeDelegate {text: "FS rootPath";info: Filesystem.rootPath}
        InfoTreeDelegate {text: "FS homePathSize";info: Filesystem.sizeInfo(Filesystem.homePath)}
        InfoTreeDelegate {text: "FS rootPathSize";info: Filesystem.sizeInfo(Filesystem.rootPath)}
        InfoTreeDelegate {text: "FS tempPath";info: Filesystem.tempPath}
        InfoTreeDelegate {text: "FS appDirPath";info: Filesystem.appDirPath}
        InfoTreeDelegate {text: "FS appDirPathSize";info: Filesystem.sizeInfo(Filesystem.appDirPath)}
        InfoTreeDelegate {text: "FS appLocalDataPath";info: Filesystem.appLocalDataPath}
        InfoTreeDelegate {text: "FS appDataPath";info: Filesystem.appDataPath}
        InfoTreeDelegate {text: "FS appCachePath";info: Filesystem.appCachePath}
        InfoTreeDelegate {text: "FS appConfigPath";info: Filesystem.appConfigPath}
        InfoTreeDelegate {text: "FS documentsPath";info: Filesystem.documentsPath}
        InfoTreeDelegate {text: "FS imagesPath";info: Filesystem.imagesPath}
        InfoTreeDelegate {text: "FS musicPath";info: Filesystem.musicPath}
        InfoTreeDelegate {text: "FS videosPath";info: Filesystem.videosPath}
        InfoTreeDelegate {text: "FS downloadsPath";info: Filesystem.downloadsPath}
        InfoTreeDelegate {text: "FS workingDirectoryPath";info: Filesystem.workingDirectoryPath}
    }
}
