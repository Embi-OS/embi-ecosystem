#include "qsdiffrunner.h"

#include "qsdiffrunneralgo.h"
#include "qspatchablepatch.h"

QVariantMap QSDiffRunner::compare(const QVariantMap& prev, const QVariantMap& current)
{
    return QSDiffRunnerAlgo::compareMap(prev, current);
}

QSPatchSet QSDiffRunner::compare(const QVariantList &from, const QVariantList &to, const QString& keyField)
{
    QSDiffRunnerAlgo algo;
    algo.setKeyField(keyField);

    const QSPatchSet patches = algo.compare(from, to);

    return patches;
}

QSPatchSet QSDiffRunner::merge(const QSPatchSet& patches)
{
    QSPatchSet reducedPatches;
    reducedPatches.reserve(patches.size());

    for(const QSPatch& patch: patches) {
        bool merged = false;

        if (reducedPatches.size() > 0) {
            if (reducedPatches.last().canMerge(patch)) {
                reducedPatches.last().merge(patch);
                merged = true;
            }
        }

        if (!merged)
            reducedPatches.append(patch);
    }

    return reducedPatches;
}

QSPatchSet QSDiffRunner::split(const QSPatchSet& patches)
{
    QSPatchablePatch patchablePatch;

    const QSPatchSet singlePatches = patchablePatch.split(patches);

    return singlePatches;
}

bool QSDiffRunner::patch(QSPatchable *patchable, const QSPatchSet& patches)
{
    bool result = true;
    for(const QSPatch& patch: patches) {
        result = QSDiffRunner::patch(patchable, patch);
        if(!result)
            break;
    }
    return result;
}

bool QSDiffRunner::patch(QSPatchable* patchable, const QSPatch& patch)
{
    bool result = true;
    switch (patch.type()) {
    case QSPatch::Remove:
        if(!patchable->remove(patch.from(), patch.count()))
            result = false;
        break;
    case QSPatch::Insert:
        if(!patchable->insert(patch.from(), patch.data()))
            result = false;
        break;
    case QSPatch::Move:
        if(!patchable->move(patch.from(), patch.to(), patch.count()))
            result = false;
        break;
    case QSPatch::Update: {
        if (patch.data().isEmpty())
            break;
        const QVariantMap diff = patch.data().at(0).toMap();
        if(!patchable->set(patch.from(), diff))
            result = false;
        break;
    }
    default:
        break;
    }
    return result;
}
