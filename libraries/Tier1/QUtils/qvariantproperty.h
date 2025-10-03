#ifndef QVARIANTPROPERTY_H
#define QVARIANTPROPERTY_H

#include <QObject>
#include <QDefs>

class QVariantProperty : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(VariantProperty)

    Q_WRITABLE_VAR_PROPERTY(int, inhibitTime, InhibitTime, 0)

    Q_PROPERTY(QVariant value READ getData WRITE setValue NOTIFY dataChanged FINAL)

    Q_WRITABLE_REF_PROPERTY(QVariant, data, Data, {})
    Q_READONLY_REF_PROPERTY(QStringList, keys, Keys, {})
    Q_READONLY_REF_PROPERTY(QVariantList, values, Values, {})
    Q_READONLY_VAR_PROPERTY(bool, isEmpty, IsEmpty, true)

public:
    explicit QVariantProperty(QObject* object=nullptr);

    Q_INVOKABLE QVariant getNestedValue(const QString& property) const;

public slots:
    void queueInvalidate();
    void invalidate();

    bool clear();
    bool setValue(const QVariant& value);
    bool setNestedValue(const QString& property, const QVariant& value);
    bool incNestedValue(const QString& property);
    bool clearNestedValue(const QString& property);

private:
    bool m_invalidateQueued = false;
    QTimer m_inhibitTimer;
};

#endif // QVARIANTPROPERTY_H
