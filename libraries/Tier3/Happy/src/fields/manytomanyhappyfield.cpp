#include "manytomanyhappyfield.h"
#include "happyserver.h"
#include "happycrudrouter.h"

ManyToManyHappyField::ManyToManyHappyField(QObject* parent):
    AbstractHappyField(parent),
    m_manyToManyModel(new QVariantListModel(this))
{
    m_manyToManyModel->setSelectPolicy(QVariantListModelPolicies::Manual);
    m_manyToManyModel->setSubmitPolicy(QVariantListModelPolicies::Disabled);
}

bool ManyToManyHappyField::init(HappyCrudRouter* crudRouter, HappyServer* happyServer)
{
    AbstractHappyField::init(crudRouter, happyServer);

    connect(m_crudRouter, &HappyCrudRouter::objectSaved, this, &ManyToManyHappyField::onObjectSaved);
    connect(m_crudRouter, &HappyCrudRouter::objectAboutToBeRemoved, this, &ManyToManyHappyField::onObjectAboutToBeRemoved);

    m_manyToManyRouter = happyServer->crudRouter(m_manyToManyName);
    m_lookupHappyField = m_crudRouter->field(m_lookupField);

    return true;
}

void ManyToManyHappyField::preRead(const HappyHttpParameters& parameters, const QSqlRecord& baseRec)
{
    AbstractHappyField::preRead(parameters, baseRec);
    m_lookupFieldIndex = baseRec.indexOf(m_lookupField);

    m_expanded = false;

    if(!m_manyToManyRouter)
        return;

    HappyHttpParameters relatedParameters;

    for(const QString& expand: parameters.expand)
    {
        if(expand=="*") {
            m_expanded = true;
            // relatedParameters.expand.append("*");
        }

        if(expand.startsWith(m_name)) {
            m_expanded = true;
            if(expand.contains("__"))
            {
                int size = expand.size()-m_name.size()-2;
                relatedParameters.expand.append(m_foreignField+"__"+expand.last(size));
            }
        }
    }

    if(m_expanded)
    {
        for(const QString& field: parameters.fields)
        {
            if(field.startsWith(m_name+"__")) {
                int size = field.size()-m_name.size()-2;
                relatedParameters.fields.append(m_foreignField+"__"+field.last(size));
            }
        }

        for(const QString& omit: parameters.omit)
        {
            if(omit.startsWith(m_name+"__")) {
                int size = omit.size()-m_name.size()-2;
                relatedParameters.omit.append(m_foreignField+"__"+omit.last(size));
            }
        }

        relatedParameters.expand.append(m_foreignField);
    }

    relatedParameters.sorters.insert(m_relatedField, Qt::AscendingOrder);

    const QVariantList list = m_manyToManyRouter->getValues(relatedParameters);

    m_manyToManyModel->clearRoleNames();
    m_manyToManyModel->setBaseName(m_manyToManyName);
    m_manyToManyModel->setSource(list);
    m_manyToManyModel->select();
    m_manyToManyModel->sort(m_relatedField, Qt::AscendingOrder);
    m_foreignFieldRole = m_manyToManyModel->roleForName(m_foreignField);
    m_relatedFieldRole = m_manyToManyModel->roleForName(m_relatedField);
}

QVariant ManyToManyHappyField::read(const QSqlRecord& record, bool* ok) const
{
    const QVariant lookupValue = m_lookupHappyField->formatRead(record.value(m_lookupFieldIndex));

    auto readRelatedField = [&](int index) {
        return m_manyToManyModel->data(m_manyToManyModel->index(index,0), m_relatedFieldRole);
    };

    auto readForeignField = [&](int index) {
        return m_manyToManyModel->data(m_manyToManyModel->index(index, 0), m_foreignFieldRole);
    };

    int index = m_manyToManyModel->indexOf(m_relatedField, lookupValue, true, Qt::AscendingOrder);

    if(index<0 || index>=m_manyToManyModel->rowCount())
        return QVariantList();

    QVariantList values;
    int upper = index;
    while(upper<m_manyToManyModel->count() && readRelatedField(upper)==lookupValue) {
        QVariant value = readForeignField(upper);
        values.append(std::move(value));
        upper++;
    }
    int lower = index-1;
    while(lower>=0 && readRelatedField(lower)==lookupValue) {
        QVariant value = readForeignField(lower);
        values.prepend(std::move(value));
        lower--;
    }

    if(ok)
        *ok = true;

    return std::move(values);
}

QVariant ManyToManyHappyField::write(const QVariantMap& values, bool creation, bool* ok) const
{
    if(!values.contains(m_name))
    {
        if(ok)
            *ok = false;
        return QVariant();
    }

    QVariant value = values.value(m_name);

    if(!value.canConvert<QVariantList>())
    {
        if(ok)
            *ok = false;
        return QVariant();
    }

    if(ok)
        *ok = true;

    QVariantList list = value.toList();
    QVariantList insert;
    insert.reserve(list.size());
    for(const QVariant& var: list)
    {
        if(var.canConvert<QVariantMap>())
            continue;
        insert.append(m_manyToManyRouter->formatFieldWrite(m_foreignField, var));
    }

    return std::move(insert);
}

void ManyToManyHappyField::onObjectSaved(const QVariant& primaryValue, const QVariantMap& object)
{
    if(!object.contains(m_name))
        return;

    const QVariant lookupValue = m_crudRouter->fieldFromPrimary(m_lookupField, primaryValue);
    const QVariant value = object.value(m_name);

    if(!value.canConvert<QVariantList>())
        return;

    SqlBuilder::delete_().from(m_manyToManyName).where(m_relatedField, lookupValue).connection(m_connection).trust().exec();

    QVariantList list = value.toList();
    QVariantList inserts;
    inserts.reserve(list.size());
    for(const QVariant& var: list)
    {
        if(var.canConvert<QVariantMap>())
            continue;
        QVariantMap insert;
        insert.insert(m_relatedField, lookupValue);
        insert.insert(m_foreignField, var);
        inserts.append(std::move(insert));
    }

    SqlBuilder::insert(inserts).into(m_manyToManyName).connection(m_connection).trust().exec();
}

void ManyToManyHappyField::onObjectAboutToBeRemoved(const QVariant& primaryValue, const QVariantMap& object)
{
    Q_UNUSED(primaryValue)

    QVariant lookupValue = m_manyToManyRouter->formatFieldWrite(m_relatedField, object.value(m_lookupField));

    SqlBuilder::delete_().from(m_manyToManyName).where(m_relatedField, lookupValue).connection(m_connection).trust().exec();
}
