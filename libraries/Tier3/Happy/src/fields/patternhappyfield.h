#ifndef PATTERNHAPPYFIELD_H
#define PATTERNHAPPYFIELD_H

#include "abstracthappyfield.h"
class PatternHappyField: public AbstractHappyField
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, pattern, Pattern, {})

public:
    explicit PatternHappyField(QObject* parent=nullptr);

    void preRead(const HappyHttpParameters& parameters, const QSqlRecord& baseRec) override;

    QVariant read(const QSqlRecord& record, bool* ok=nullptr) const override;
    QVariant write(const QVariantMap& values, bool creation, bool* ok=nullptr) const override;

private:
    QString m_string;
    QStringList m_captures;
    QStringList m_roleNames;
    QList<int> m_indexes;
    QList<int> m_precisions;
};

#endif // PATTERNHAPPYFIELD_H
