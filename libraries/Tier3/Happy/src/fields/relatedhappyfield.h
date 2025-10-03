#ifndef RELATEDHAPPYFIELD_H
#define RELATEDHAPPYFIELD_H

#include "abstracthappyfield.h"

class HappyCrudRouter;
class RelatedHappyField: public AbstractHappyField
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, lookupField, LookupField, {})
    Q_WRITABLE_REF_PROPERTY(QString, relatedName, RelatedName, {})
    Q_WRITABLE_REF_PROPERTY(QString, relatedField, RelatedField, {})
    Q_WRITABLE_REF_PROPERTY(QString, relatedKey, RelatedKey, {})

public:
    explicit RelatedHappyField(QObject* parent=nullptr);

    bool init(HappyCrudRouter* crudRouter, HappyServer* happyServer) override;

    void preRead(const HappyHttpParameters& parameters, const QSqlRecord& baseRec) override;

    QVariant read(const QSqlRecord& record, bool* ok=nullptr) const override;
    QVariant write(const QVariantMap& values, bool creation, bool* ok=nullptr) const override;

private:
    bool m_expanded=false;
    QVariantListModel* m_relatedModel=nullptr;
    AbstractHappyField* m_lookupHappyField=nullptr;
    HappyCrudRouter* m_relatedRouter=nullptr;
    int m_lookupFieldIndex;
    int m_relatedKeyRole;
    int m_relatedFieldRole;
};

#endif // RELATEDHAPPYFIELD_H
