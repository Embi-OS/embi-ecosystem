#include "qsdiffrunneralgo.h"
#include "qmodels_log.h"

using namespace QSAlgoTypes;

QSDiffRunnerAlgo::QSDiffRunnerAlgo()
{
    insertStart = -1;
    removeStart = -1;

    skipped = 0;
    indexT = -1;
    indexF = -1;

    removing = 0;
}

QSPatchSet QSDiffRunnerAlgo::compareWithoutKey(const QVariantList &from, const QVariantList &to, int start)
{
    QSPatchSet patches;
    patches.reserve(from.size()+2);

    int max = qMax(from.size(), to.size());

    for (int i = start ; i < max ; i++) {
        if (i >= from.size()) {
            const QVariantList diff = to.last(to.size()-i);
            patches << QSPatch::createInsert(i, diff);
            break;
        } else if (i >= to.size() ) {
            patches << QSPatch::createRemove(i, from.size()-1);
            break;
        } else {
            const QVariantMap diff = compareMap(from.at(i).toMap(), to.at(i).toMap());
            if (!diff.isEmpty()) {
                patches << QSPatch::createUpdate(i, diff);
            }
        }
    }

    return patches;
}

QVariantMap QSDiffRunnerAlgo::compareMap(const QVariantMap &prev, const QVariantMap &current)
{
    QVariantMap diff;

    if(prev==current)
        return diff;

    for (auto [key, value] : current.asKeyValueRange())
    {
        const QVariant& oldValue = prev[key];
        if(oldValue!=value || !prev.contains(key))
            diff[key] = value;
    }

    return diff;
}

QString QSDiffRunnerAlgo::keyField() const
{
    return m_keyField;
}

void QSDiffRunnerAlgo::setKeyField(const QString &keyField)
{
    m_keyField = keyField;
}

QSPatchSet QSDiffRunnerAlgo::combine()
{
    if (updatePatches.size() > 0) {
        patches.append(updatePatches);
    }

    return patches;
}

int QSDiffRunnerAlgo::preprocess(const QVariantList &from, const QVariantList &to)
{
    int index = 0;
    int min = qMin(from.size(), to.size());

    for (index = 0 ; index < min ;index++) {

        const QVariantMap f = from[index].toMap();
        const QVariantMap t = to[index].toMap();

        if (f[m_keyField] != t[m_keyField]) {
            break;
        }

        const QVariantMap diff = compareMap(f, t);
        if (diff.size()) {
            //@TODO reserve in block size
            updatePatches << QSPatch::createUpdate(index, diff);
        }
    }

    if (from.size() == index && to.size() - index > 0)  {
        // Special case: append to end
        skipped = to.size();
        appendPatch(createInsertPatch(index,to.size() - 1,to));
        return to.size();
    }

    if (to.size() == index && from.size() - index> 0) {
        // Special case: removed from end
        appendPatch(QSPatch::createRemove(index, from.size() - 1));
        skipped = from.size();
        return from.size();
    }

    skipped = index;

    return index;
}

bool QSDiffRunnerAlgo::buildHashTable()
{
    hash.reserve((qMax(to.size(), from.size()) - skipped) * 2 + 100);

    State state;
    int fromSize = from.size();
    int toSize = to.size();

    for (int i = skipped; i < fromSize ; i++) {
        const QVariantMap item = from.at(i).toMap();
        if(!item.contains(m_keyField)) {
            QMODELSLOG_DEBUG() << "Missing key, fallback to compareWithoutKey";
            return false;
        }
        const QString key = item[m_keyField].toString();
        if (hash.contains(key)) {
            QMODELSLOG_DEBUG() << "Duplicated key:"<<key<<"fallback to compareWithoutKey";
            return false;
        }
        state.posF = i;
        state.posT = -1;
        hash.insert(key, state);
    }

    for (int i = skipped; i < toSize ; i++) {
        const QVariantMap item = to.at(i).toMap();
        const QString key = item[m_keyField].toString();

        if (hash.contains(key)) {
            hash[key].posT = i;
        } else {
            state.posF = -1;
            state.posT = i;
            hash.insert(key, state);
        }
    }

    return true;
}

void QSDiffRunnerAlgo::appendPatch(const QSPatch &value, bool merge)
{
    bool merged = false;

    if (patches.size() > 0 && merge) {
        if (patches.last().canMerge(value)) {
            patches.last().merge(value);
            merged = true;
        }
    }

    if (!merged) {
        patches << value;
    }
}

QSPatch QSDiffRunnerAlgo::createInsertPatch(int from, int to, const QVariantList& source)
{
    int count = to - from + 1;

    return QSPatch(QSPatch::Insert, from, to, count, source.mid(from, count));
}

void QSDiffRunnerAlgo::appendMovePatch(QSAlgoTypes::MoveOp &moveOp)
{

    QSPatch patch(QSPatch::Move, moveOp.from, moveOp.to, moveOp.count);

    int offset = 0;

    QSTreeNode* node = tree.insert(moveOp.posF,moveOp.count);
    offset = tree.countLessThan(node);

    if (offset > 0) {
        patch.setFrom(patch.from() - offset);
    }

    appendPatch(patch);
}

void QSDiffRunnerAlgo::appendRemovePatches()
{
    appendPatch(QSPatch::createRemove(indexT,
                                      indexT + removing - 1), false);

    removeStart = -1;
    removing = 0;
}

void QSDiffRunnerAlgo::updateTree()
{
    while (tree.root() != 0 && tree.min() <= indexF) {
        tree.remove(tree.min());
    }
}

QSPatchSet QSDiffRunnerAlgo::compare(const QVariantList& from, const QVariantList& to)
{
    if (m_keyField.isEmpty()) {
        return compareWithoutKey(from, to);
    }

    patches.clear();
    patches.reserve(from.size()+2);
    updatePatches.clear();
    updatePatches.reserve(from.size()+2);

    this->from = from;
    this->to = to;

    // Compare the list, until it found moved component.
    preprocess(from, to);

    if (skipped >= from.size() &&
        skipped >= to.size()) {
        // Nothing moved
        return combine();
    }

    if(!buildHashTable()) {
        patches = compareWithoutKey(from, to, skipped);
        return combine();
    }

    indexF = skipped;
    indexT = skipped;
    int fromSize = from.size();
    int toSize = to.size();

    State state;

    while (indexF < fromSize || indexT < toSize) {

        keyF.clear();

        while (indexF < fromSize) {
            // Process until it found an item that remain in origianl position (neither removd / moved).
            itemF = from.at(indexF).toMap();
            keyF = itemF[m_keyField].toString();
            state = hash[keyF]; // It mush obtain the key value


            if (state.posT < 0) {
                markItemAtFromList(Remove, state);
                indexF++;
            } else if (state.isMoved) {
                markItemAtFromList(Move, state);
                indexF++;
            } else {
                markItemAtFromList(NoMove, state);
                // The item remain in original position.
                break;
            }
        }

        if (indexF >= fromSize && indexT < toSize) {
            // The rest in "to" list is new items
            appendPatch(createInsertPatch(indexT, toSize - 1, to), false);
            return combine();
        }

        while (indexT < toSize ) {
            itemT = to.at(indexT).toMap();
            keyT = itemT[m_keyField].toString();
            state = hash[keyT];

            if (state.posF < 0) {
                // new item
                markItemAtToList(Insert, state);
                indexT++;
            } else {
                if (keyT != keyF) {
                    markItemAtToList(Move, state);
                    indexT++;
                } else {
                    markItemAtToList(NoMove, state);
                    indexT++;
                    indexF++;
                    break;
                }
            }
        }
    }
    State dummy;

    markItemAtToList(NoMove, dummy);
    markItemAtFromList(NoMove, dummy);

    return combine();
}

void QSDiffRunnerAlgo::markItemAtFromList(QSAlgoTypes::Type type, State &state)
{
    if (removeStart >= 0 && type != QSAlgoTypes::Remove) {
        appendRemovePatches();
    }

    if (type == QSAlgoTypes::Remove) {
        if (removeStart < 0) {
            removeStart = indexF;
        }
        removing++;

        if (indexF == from.size() - 1) {
            // It is the last item
            appendRemovePatches();
        }
    }

    if (type == Move) {
        updateTree();
    }

    state.posF = indexF;
    hash[keyF] = state;
}

void QSDiffRunnerAlgo::markItemAtToList(QSAlgoTypes::Type type, State& state)
{

    if (insertStart >= 0 && type != QSAlgoTypes::Insert) {
        /* Insert */
        appendPatch(createInsertPatch(insertStart,
                                     indexT - 1, to), false);
        insertStart = -1;
    }

    if (type == QSAlgoTypes::Insert) {
        if (insertStart < 0) {
            insertStart = indexT;
        }
    }

    if (type == QSAlgoTypes::Move) {
        MoveOp change(state.posF,
                      indexT + state.posF - indexF,
                      indexT);

        if (pendingMovePatch.isNull()) {
            pendingMovePatch = change;
        } else if (pendingMovePatch.canMerge(change)){
            pendingMovePatch.merge(change);
        } else {
            appendMovePatch(pendingMovePatch);
            pendingMovePatch = change;
        }

        state.isMoved = true;
        hash[keyT] = state;
    }

    if (type != QSAlgoTypes::Move && !pendingMovePatch.isNull()) {
        appendMovePatch(pendingMovePatch);
        pendingMovePatch.clear();
    }

    if (indexT < to.size() && (type == QSAlgoTypes::Move || type == QSAlgoTypes::NoMove)) {
        const QVariantMap tmpItemF = from[state.posF].toMap();
        const QVariantMap diff = compareMap(tmpItemF, itemT);
        if (diff.size()) {
            updatePatches << QSPatch(QSPatch::Update, indexT, indexT, 1, diff);
        }
    }
}
