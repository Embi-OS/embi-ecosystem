#ifndef FOREIGNHAPPYFIELD_H
#define FOREIGNHAPPYFIELD_H

#include "varhappyfield.h"

class HappyCrudRouter;
class ForeignHappyField: public VarHappyField
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, foreignName, ForeignName, {})
    Q_WRITABLE_REF_PROPERTY(QString, foreignField, ForeignField, {})

    Q_WRITABLE_REF_PROPERTY(QVariantMap, foreignFilters, ForeignFilters, {})

public:
    explicit ForeignHappyField(QObject* parent=nullptr);

    bool init(HappyCrudRouter* crudRouter, HappyServer* happyServer) override;

    void preRead(const HappyHttpParameters& parameters, const QSqlRecord& baseRec) override;

    QVariant read(const QSqlRecord& record, bool* ok=nullptr) const override;

private:
    bool m_expanded=false;
    QVariantListModel* m_foreignModel=nullptr;
    HappyCrudRouter* m_foreignRouter=nullptr;
    int m_foreignRole=-1;
};

#endif // FOREIGNHAPPYFIELD_H
