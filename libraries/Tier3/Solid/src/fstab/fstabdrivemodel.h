#ifndef FSTABDRIVEMODEL_H
#define FSTABDRIVEMODEL_H

#include "files/filesystemdrivemodel.h"
#include "fstab.h"

class FstabDriveModel : public FilesystemDriveModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_VAR_PROPERTY(bool, showFsTabDrives, ShowFsTabDrives, false)

public:
    explicit FstabDriveModel(QObject* parent = nullptr);

protected slots:
    void updateStorageList(const QList<QStorageInfo>& storageList) override;

protected:
    bool isFstabEntryValid(const FstabEntry& entry);
};

#endif // FSTABDRIVEMODEL_H
