#ifndef QSPATCHABLE
#define QSPATCHABLE

#include <QVariant>

class QSPatchable {
public:

    virtual bool insert(int index, const QVariant& variant) = 0;

    virtual bool move(int from, int to, int count = 1) = 0;

    virtual bool remove(int index, int count = 1) = 0;

    virtual bool set(int index, const QVariant& variant) = 0;
};

#endif // QSPATCHABLE

