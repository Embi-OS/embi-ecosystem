#ifndef QSPATCH_H
#define QSPATCH_H

#include <QDefs>

class QSPatch {
    Q_GADGET
    QML_VALUE_TYPE(qsPatch)
    QML_STRUCTURED_VALUE

public:
    enum Type {
        Null,
        Insert,
        Remove,
        Update,
        Move
    };

    QSPatch() = default;
    ~QSPatch() = default;
    QSPatch(Type type,int from = 0, int to = 0, int count = 0);
    QSPatch(Type type,int from, int to, int count, const QVariantMap& data);
    QSPatch(Type type,int from, int to, int count, const QVariantList& data);
    QSPatch(const QSPatch& other);
    QSPatch &operator=(const QSPatch& other);

    QSPatch::Type type() const;
    void setType(const QSPatch::Type &type);

    const QVariantList& data() const;

    void setData(const QVariantList &data);
    void setData(const QVariantMap& data);

    bool operator==(const QSPatch& rhs) const;

    int from() const;
    void setFrom(int from);

    int to() const;
    void setTo(int to);

    bool isNull() const;

    int count() const;
    void setCount(int count);

    bool canMerge(const QSPatch& other) const;

    QSPatch merged(const QSPatch &other) const;
    QSPatch& merge(const QSPatch &other);

    static QSPatch createUpdate(int index, const QVariantMap& diff);
    static QSPatch createRemove(int from, int to);
    static QSPatch createInsert(int index, const QVariantMap& map);
    static QSPatch createInsert(int index, const QVariantList& list);

private:
    QSPatch::Type m_type;
    int m_from;
    int m_to;
    int m_count;
    QVariantList m_data;
};

QDebug operator<<(QDebug dbg, const QSPatch& patch);

typedef QList<QSPatch> QSPatchSet;

#endif // QSPATCH_H
