#include "foreignhappyfield.h"
#include "happyserver.h"
#include "happycrudrouter.h"

ForeignHappyField::ForeignHappyField(QObject* parent):
    VarHappyField(parent),
    m_foreignModel(new QVariantListModel(this))
{
    m_foreignModel->setSelectPolicy(QVariantListModelPolicies::Manual);
    m_foreignModel->setSubmitPolicy(QVariantListModelPolicies::Disabled);
}

bool ForeignHappyField::init(HappyCrudRouter* crudRouter, HappyServer* happyServer)
{
    AbstractHappyField::init(crudRouter, happyServer);

    m_foreignRouter = happyServer->crudRouter(m_foreignName);

    return true;
}

void ForeignHappyField::preRead(const HappyHttpParameters& parameters, const QSqlRecord& baseRec)
{
    AbstractHappyField::preRead(parameters, baseRec);

    m_expanded = false;

    if(!m_foreignRouter)
        return;

    HappyHttpParameters foreignParameters;

    for(const QString& expand: parameters.expand)
    {
        if(expand=="*") {
            m_expanded = true;
            // foreignParameters.expand.append("*");
        }

        if(expand.startsWith(m_name)) {
            m_expanded = true;
            if(expand.contains("__"))
            {
                int size = expand.size()-m_name.size()-2;
                foreignParameters.expand.append(expand.last(size));
            }
        }
    }

    if(!m_expanded)
        return;

    if(m_readMutex.tryLock())
    {
        for(const QString& field: parameters.fields)
        {
            if(field.startsWith(m_name+"__")) {
                int size = field.size()-m_name.size()-2;
                foreignParameters.fields.append(field.last(size));
            }
        }

        for(const QString& omit: parameters.omit)
        {
            if(omit.startsWith(m_name+"__")) {
                int size = omit.size()-m_name.size()-2;
                foreignParameters.omit.append(omit.last(size));
            }
        }

        foreignParameters.sorters.insert(m_foreignField, Qt::AscendingOrder);
        foreignParameters.filters = m_foreignFilters;;

        const QVariantList list = m_foreignRouter->getValues(foreignParameters);

        m_foreignModel->clearRoleNames();
        m_foreignModel->setBaseName(m_foreignName);
        m_foreignModel->setSource(list);
        m_foreignModel->select();
        m_foreignModel->sort(m_foreignField);
        m_foreignRole = m_foreignModel->roleForName(m_foreignField);

        m_expanded = true;

        m_readMutex.unlock();
    }
    else
    {
        m_expanded = false;
    }
}

QVariant ForeignHappyField::read(const QSqlRecord& record, bool* ok) const
{
    QVariant value = formatRead(record.value(m_nameIndex), ok);

    if(m_expanded)
    {
        const int index = QModelHelper::indexOf(m_foreignModel, m_foreignRole, value, true);
        if(index<0 || index>=m_foreignModel->rowCount())
            return QVariant();

        value = m_foreignModel->get(index);
    }

    if(ok)
        *ok = true;

    return value;
}
