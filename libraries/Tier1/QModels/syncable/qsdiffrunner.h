#ifndef QSDIFFRUNNER_H
#define QSDIFFRUNNER_H

#include "qspatch.h"
#include "qspatchable.h"

class QSDiffRunner
{
public:
    QSDiffRunner() = default;

    static QVariantMap compare(const QVariantMap& prev, const QVariantMap& current);

    static QSPatchSet compare(const QVariantList& from, const QVariantList& to, const QString &keyField="");

    static QSPatchSet merge(const QSPatchSet& patches);
    static QSPatchSet split(const QSPatchSet& patches);

    static bool patch(QSPatchable* patchable, const QSPatchSet& patches);
    static bool patch(QSPatchable* patchable, const QSPatch& patch);
};

#endif // QSDIFFRUNNER_H
