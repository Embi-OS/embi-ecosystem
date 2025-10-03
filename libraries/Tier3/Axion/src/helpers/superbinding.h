#ifndef SUPERBINDING_H
#define SUPERBINDING_H

#include <QDefs>

#include <QJSValue>
#include <QQmlProperty>
#include <QQmlParserStatus>
#include <QQmlPropertyValueSource>

Q_ENUM_CLASS(SuperBindingPolicies, SuperBindingPolicy,
             Direct,
             Delayed,
             Manual,
             Disabled)

class SuperBinding : public QObject,
                     public QQmlParserStatus,
                     public QQmlPropertyValueSource
{
    Q_OBJECT
    QML_ELEMENT

    Q_INTERFACES (QQmlParserStatus)
    Q_INTERFACES (QQmlPropertyValueSource)

    Q_WRITABLE_VAR_PROPERTY(SuperBindingPolicies::Enum, policy, Policy, SuperBindingPolicies::Direct)
    Q_WRITABLE_VAR_PROPERTY(bool, when, When, true)
    Q_WRITABLE_VAR_PROPERTY(bool, invalidateWhen, InvalidateWhen, true)
    Q_WRITABLE_VAR_PROPERTY(int, inhibitTime, InhibitTime, 0)
    Q_WRITABLE_REF_PROPERTY(QVariant, value, Value, {})

    Q_READONLY_VAR_PROPERTY(bool, pending, Pending, false)

public:
    explicit SuperBinding(QObject* parent=nullptr);

    void classBegin() override final;
    void componentComplete() override final;
    void setTarget(const QQmlProperty& target) override final;

public slots:
    void queueInvalidate();
    void invalidate();

signals:
    void completedChanged(bool completed);

    void invalidateDone();

protected slots:
    void onInhibitTimeChanged(int inhibitTime);

    void doInvalidate();

private:
    bool m_completed=false;
    bool m_invalidateQueued = false;
    QTimer m_inhibitTimer;
    QQmlProperty m_property;
};

#endif // SUPERBINDING_H
