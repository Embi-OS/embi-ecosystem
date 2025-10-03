#include "qspatchablepatch.h"
#include "qsdiffrunner.h"

const QSPatchSet& QSPatchablePatch::split(const QSPatchSet& patches)
{
    m_singlePatches.clear();

    bool result = QSDiffRunner::patch(this, patches);

    if(!result)
        m_singlePatches.clear();

    return m_singlePatches;
}

bool QSPatchablePatch::insert(int index, const QVariant& variant)
{
    QVariantList variants;
    if(variant.metaType().id()!=QMetaType::QString && variant.canConvert<QVariantList>())
        variants = variant.toList();
    else
        variants.append(variant);

    QSPatchSet patches;
    patches.reserve(variants.count());

    for(const QVariant& var: variants)
    {
        QVariantMap map = var.toMap();
        const QSPatch patch = QSPatch::createInsert(index, map);
        patches.append(patch);
        index++;
    }

    m_singlePatches.append(patches);

    return true;
}

bool QSPatchablePatch::set(int index, const QVariant& variant)
{
    const QVariantMap map = variant.toMap();
    const QSPatch patch = QSPatch::createUpdate(index, map);
    m_singlePatches.append(patch);

    return true;
}

bool QSPatchablePatch::move(int from, int to, int count)
{
    QSPatchSet patches;
    patches.reserve(count);

    for(int i = 0; i < count; ++i)
    {
        const QSPatch patch = QSPatch(QSPatch::Move, from, to, 1);
        patches.append(patch);
    }

    m_singlePatches.append(patches);

    return true;
}

bool QSPatchablePatch::remove(int index, int count)
{
    QSPatchSet patches;
    patches.reserve(count);

    for(int i = 0; i < count; ++i)
    {
        const QSPatch patch = QSPatch::createRemove(index, index);
        patches.append(patch);
    }

    m_singlePatches.append(patches);

    return true;
}
