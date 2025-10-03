#ifndef RESTPATCHABLE_H
#define RESTPATCHABLE_H

#include <QDefs>

#include "syncable/qspatchable.h"
#include "syncable/qspatch.h"

class QSPatchablePatch : public QSPatchable
{
public:
    QSPatchablePatch() = default;

    const QSPatchSet& split(const QSPatchSet& patches);

    bool insert(int index, const QVariant& variant) final override;
    bool set(int index, const QVariant& variant) final override;
    bool move(int from, int to, int count = 1) final override;
    bool remove(int index, int count = 1) final override;

private:
    QSPatchSet m_singlePatches;
};

#endif // SQLPATCHABLE_H
