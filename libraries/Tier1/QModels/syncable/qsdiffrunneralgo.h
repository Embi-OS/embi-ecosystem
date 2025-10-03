#ifndef QSDIFFRUNNERALGO_H
#define QSDIFFRUNNERALGO_H

#include "qsalgotypes.h"
#include "qspatch.h"
#include "qstree.h"

class QSDiffRunnerAlgo {

public:
    QSDiffRunnerAlgo();

    QSPatchSet compare(const QVariantList& from, const QVariantList& to);

    static QSPatchSet compareWithoutKey(const QVariantList& from, const QVariantList& to, int start=0);

    static QVariantMap compareMap(const QVariantMap& prev, const QVariantMap& current);

    QString keyField() const;
    void setKeyField(const QString &keyField);

private:
    // Combine all the processing patches into a single list. It will clear the processing result too.
    QSPatchSet combine();

    // Preprocess the list, stop until the key is different. It will also handle common pattern (like append to end , remove from end)
    int preprocess(const QVariantList& from, const QVariantList& to);

    bool buildHashTable();

    // Mark an item for insert, remove, move
    void markItemAtFromList(QSAlgoTypes::Type type, QSAlgoTypes::State &state);

    void markItemAtToList(QSAlgoTypes::Type type, QSAlgoTypes::State& state);

    static QSPatch createInsertPatch(int from, int to, const QVariantList& source );

    void appendPatch(const QSPatch& patch, bool merge = true);

    void appendMovePatch(QSAlgoTypes::MoveOp& patch);

    void appendRemovePatches();

    void updateTree();

    QVariantList from;

    QVariantList to;

    // Stored patches (without any update patches)
    QSPatchSet patches;

    // Update patches
    QSPatchSet updatePatches;

    // Hash table
    QHash<QString, QSAlgoTypes::State> hash;

    // The start position of remove block
    int removeStart;

    // No. of item removing
    int removing;

    // The start position of insertion block
    int insertStart;

    // A no. of item could be skipped found preprocess().
    int skipped;

    QString keyF,keyT;

    int indexF,indexT;

    QVariantMap itemF,itemT;

    /* Move Patches */
    QSAlgoTypes::MoveOp pendingMovePatch;

    // Tree of move patch
    QSTree tree;

    QString m_keyField;
};

#endif // QSDIFFRUNNERALGO_H
