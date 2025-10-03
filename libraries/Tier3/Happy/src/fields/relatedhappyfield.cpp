#include "relatedhappyfield.h"
#include "happyserver.h"
#include "happycrudrouter.h"

RelatedHappyField::RelatedHappyField(QObject* parent):
    AbstractHappyField(parent),
    m_relatedModel(new QVariantListModel(this))
{
    m_relatedModel->setSelectPolicy(QVariantListModelPolicies::Manual);
    m_relatedModel->setSubmitPolicy(QVariantListModelPolicies::Disabled);
}

bool RelatedHappyField::init(HappyCrudRouter* crudRouter, HappyServer* happyServer)
{
    AbstractHappyField::init(crudRouter, happyServer);

    m_relatedRouter = happyServer->crudRouter(m_relatedName);
    m_lookupHappyField = m_crudRouter->field(m_lookupField);

    return true;
}

void RelatedHappyField::preRead(const HappyHttpParameters& parameters, const QSqlRecord& baseRec)
{
    AbstractHappyField::preRead(parameters, baseRec);
    m_lookupFieldIndex = baseRec.indexOf(m_lookupField);

    m_expanded = false;

    if(!m_relatedRouter)
        return;

    HappyHttpParameters relatedParameters;

    for(const QString& expand: parameters.expand)
    {
        if(expand=="*") {
            m_expanded = true;
            //relatedParameters.expand.append("*");
        }

        if(expand.startsWith(m_name)) {
            m_expanded = true;
            if(expand.contains("__"))
            {
                int size = expand.size()-m_name.size()-2;
                relatedParameters.expand.append(expand.last(size));
            }
        }
    }

    if(m_expanded)
    {
        for(const QString& field: parameters.fields)
        {
            if(field.startsWith(m_name+"__")) {
                int size = field.size()-m_name.size()-2;
                relatedParameters.fields.append(field.last(size));
            }
        }

        for(const QString& omit: parameters.omit)
        {
            if(omit.startsWith(m_name+"__")) {
                int size = omit.size()-m_name.size()-2;
                relatedParameters.omit.append(omit.last(size));
            }
        }
    }

    relatedParameters.sorters.insert(m_relatedField, Qt::AscendingOrder);

    const QVariantList list = m_relatedRouter->getValues(relatedParameters);

    m_relatedModel->clearRoleNames();
    m_relatedModel->setBaseName(m_relatedName);
    m_relatedModel->setSource(list);
    m_relatedModel->select();
    m_relatedModel->sort(m_relatedField);
    m_relatedKeyRole = m_relatedModel->roleForName(m_relatedKey);
    m_relatedFieldRole = m_relatedModel->roleForName(m_relatedField);
}

QVariant RelatedHappyField::read(const QSqlRecord& record, bool* ok) const
{
    const QVariant lookupValue = m_lookupHappyField->formatRead(record.value(m_lookupFieldIndex));

    auto readRelatedField = [&](int index) {
        return m_relatedModel->data(m_relatedModel->index(index,0), m_relatedFieldRole);
    };

    auto readRelatedKey = [&](int index) {
        if(m_expanded)
            return m_relatedModel->get(index);
        else
            return m_relatedModel->data(m_relatedModel->index(index, 0), m_relatedKeyRole);
    };

    int index = QModelHelper::indexOf(m_relatedModel, m_relatedFieldRole, lookupValue, true);
    if(index<0 || index>=m_relatedModel->rowCount())
        return QVariantList();

    QVariantList values;
    int upper = index;
    while(upper<m_relatedModel->count() && readRelatedField(upper)==lookupValue) {
        QVariant value = readRelatedKey(upper);
        values.append(std::move(value));
        upper++;
    }
    int lower = index-1;
    while(lower>=0 && readRelatedField(lower)==lookupValue) {
        QVariant value = readRelatedKey(lower);
        values.prepend(std::move(value));
        lower--;
    }

    if(ok)
        *ok = true;

    return std::move(values);
}

QVariant RelatedHappyField::write(const QVariantMap& values, bool creation, bool* ok) const
{
    if(ok)
        *ok = false;

    return QVariant();
}
