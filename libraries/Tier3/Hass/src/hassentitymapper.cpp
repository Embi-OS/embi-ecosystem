#include "hassentitymapper.h"

HassEntityMapper::HassEntityMapper(QObject* parent):
    RestMapper("api/states", parent, &HassEntityMapper::staticMetaObject)
{
    m_selectPolicy = QVariantMapperPolicies::Delayed;
    m_submitPolicy = QVariantMapperPolicies::Disabled;
}
