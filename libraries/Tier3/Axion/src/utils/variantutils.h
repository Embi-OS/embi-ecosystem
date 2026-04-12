#ifndef VARIANTUTILS_H
#define VARIANTUTILS_H

#include <QDefs>
#include "qsingleton.h"

class VariantUtils : public QObject,
                     public QQmlSingleton<VariantUtils>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_CONSTANT_REF_PROPERTY(QVariantMap, emptyVariantMap, {})
    Q_CONSTANT_REF_PROPERTY(QVariantList, emptyVariantList, {})

protected:
    friend QQmlSingleton<VariantUtils>;
    explicit VariantUtils(QObject *parent = nullptr);

public:
    Q_INVOKABLE static QVariant fromJSVariant(const QVariant& variant);
    Q_INVOKABLE static QVariant getNestedValue(const QVariant& variant, const QString& keys, const QVariant& v=QVariant());
};

#endif // VARIANTUTILS_H
