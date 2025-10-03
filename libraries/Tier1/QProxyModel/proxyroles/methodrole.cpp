#include "methodrole.h"
#include "qqmlsortfilterproxymodel.h"
#include "qproxymodel_log.h"

namespace QQsfpm {

MethodRole::MethodRole(QObject *parent) : SingleRole(parent)
{
    connect(this, &MethodRole::customRoleNamesChanged, this, &MethodRole::roleNamesInvalidated);
}

void MethodRole::componentComplete()
{
    updateMethod();
}

void MethodRole::updateRoles(const QQmlSortFilterProxyModel& proxyModel)
{
    m_parameterRoles.clear();
    for(const QByteArray& parameterName: std::as_const(m_parameterNames))
    {
        if(parameterName=="model")
            m_parameterRoles << -1;
        else
            m_parameterRoles << proxyModel.roleForName(parameterName);
    }

    m_roleNames = proxyModel.roleNames();
    if(!m_customRoleNames.isEmpty())
    {
        const QHash<int,QByteArray> roleNames = proxyModel.roleNames();
        m_roleNames.clear();
        m_roleNames.reserve(m_customRoleNames.size());
        for(const QString& customRoleName: std::as_const(m_customRoleNames))
        {
            const QByteArray roleName = customRoleName.toUtf8();
            m_roleNames[roleNames.key(roleName)] = roleName;
        }
    }
}

QVariant MethodRole::data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel)
{
    if(!m_method.isValid() || m_parameterRoles.isEmpty())
        return QVariant();

    QVariant ret(m_returnType);
    std::vector<void*> argv;
    argv.push_back(&ret);

    QVariantList params;
    for(int i=0; i<m_parameterRoles.size(); i++)
    {
        const int parameterRole = m_parameterRoles.at(i);

        QVariant param;
        if(parameterRole<0)
        {
            QVariantMap modelMap;
            for(auto [key, value]: m_roleNames.asKeyValueRange())
                modelMap.insert(value, proxyModel.sourceData(sourceIndex, key));
            param = modelMap;
        }
        else
        {
            const QMetaType parameterType = m_parameterTypes.at(i);
            param = proxyModel.sourceData(sourceIndex, parameterRole);
            if(parameterType.id()!=QMetaType::QVariant && !param.canConvert(parameterType)) {
                QPROXYMODELLOG_WARNING()<<"Unable to convert param type"<<param.metaType()<<"to"<<parameterType;
                return m_defaultValue;
            }
            else if(parameterType.id()!=QMetaType::QVariant) {
                param.convert(parameterType);
            }
        }

        params.append(param);
    }

    for(QVariant& param: params)
        argv.push_back(param.data());

    QMetaObject::metacall(const_cast<MethodRole*>(this), QMetaObject::InvokeMetaMethod, m_method.methodIndex(), argv.data());

    return ret;
}

void MethodRole::updateMethod()
{
    const auto *metaObj = metaObject();
    for (int idx = metaObj->methodOffset(); idx < metaObj->methodCount(); idx++) {
        // Once we find the method signature, break the loop
        QMetaMethod method = metaObj->method(idx);
        if (method.name() == "method") {
            m_method = method;
            break;
        }
    }

    if (!m_method.isValid())
        return;

    if (m_method.parameterCount() < 1) {
        qWarning()<<"TableViewMethodColumn method requires at least one parameter";
        return;
    }

    m_parameterNames = m_method.parameterNames();
    m_parameterTypes.clear();
    for(int i=0; i<m_method.parameterCount(); i++)
        m_parameterTypes << m_method.parameterMetaType(i);
    m_returnType = m_method.returnMetaType();

    invalidate();
}

}
