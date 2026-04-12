#include "variantutils.h"

VariantUtils::VariantUtils(QObject *parent) :
    QObject(parent)
{

}

QVariant VariantUtils::fromJSVariant(const QVariant& variant)
{
    return qVariantFromJSVariant(variant);
}

QVariant VariantUtils::getNestedValue(const QVariant& variant, const QString& keys, const QVariant& v)
{
    bool result = false;
    const QVariant ret = qVariantGetNestedValue(qVariantFromJSVariant(variant), keys, ".", &result);
    if(!result)
        return v;
    return ret;
}
