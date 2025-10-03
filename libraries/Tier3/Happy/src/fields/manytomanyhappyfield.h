#ifndef MANYTOMANYHAPPYFIELD_H
#define MANYTOMANYHAPPYFIELD_H

#include "abstracthappyfield.h"

class HappyCrudRouter;
class ManyToManyHappyField: public AbstractHappyField
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, manyToManyName, ManyToManyName, {})
    Q_WRITABLE_REF_PROPERTY(QString, lookupField, LookupField, {})
    Q_WRITABLE_REF_PROPERTY(QString, relatedField, RelatedField, {})
    Q_WRITABLE_REF_PROPERTY(QString, foreignField, ForeignField, {})

public:
    explicit ManyToManyHappyField(QObject* parent=nullptr);

    bool init(HappyCrudRouter* crudRouter, HappyServer* happyServer) override;

    void preRead(const HappyHttpParameters& parameters, const QSqlRecord& baseRec) override;

    QVariant read(const QSqlRecord& record, bool* ok=nullptr) const override;
    QVariant write(const QVariantMap& values, bool creation, bool* ok=nullptr) const override;

protected slots:
    void onObjectSaved(const QVariant& primaryValue, const QVariantMap& object);
    void onObjectAboutToBeRemoved(const QVariant& primaryValue, const QVariantMap& object);

private:
    bool m_expanded=false;
    QVariantListModel* m_manyToManyModel=nullptr;
    AbstractHappyField* m_lookupHappyField=nullptr;
    HappyCrudRouter* m_manyToManyRouter=nullptr;
    int m_lookupFieldIndex;
    int m_foreignFieldRole;
    int m_relatedFieldRole;
};

#endif // MANYTOMANYHAPPYFIELD_H
