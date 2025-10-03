#ifndef QSIGNALTHROTTLER_H
#define QSIGNALTHROTTLER_H

#include <QObject>
#include <QDefs>

class QGenericSignalThrottler : public QObject
{
public:
    enum Type {
        Throttler,
        Debouncer,
    };
    Q_ENUM(Type)

    enum EmissionPolicy {
        Trailing,
        Leading,
    };
    Q_ENUM(EmissionPolicy)

private:
    Q_OBJECT
    QML_NAMED_ELEMENT(SignalThrottler)
    QML_UNCREATABLE("Abstract")

    Q_CONSTANT_VAR_PROPERTY(QGenericSignalThrottler::Type, type, Type, {})
    Q_CONSTANT_VAR_PROPERTY(QGenericSignalThrottler::EmissionPolicy, emissionPolicy, EmissionPolicy, {})

    Q_PROPERTY(int timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged FINAL)
    Q_PROPERTY(Qt::TimerType timerType READ timerType WRITE setTimerType NOTIFY timerTypeChanged FINAL)

public:
    explicit QGenericSignalThrottler(Type type, EmissionPolicy emissionPolicy, QObject *parent = nullptr);
    ~QGenericSignalThrottler() override;

    int timeout() const;
    Qt::TimerType timerType() const;

public slots:
    void throttle();
    void cancel();
    void setTimeout(int timeout);
    void setTimerType(Qt::TimerType timerType);

signals:
    void triggered();
    void timeoutChanged(int timeout);
    void timerTypeChanged(Qt::TimerType timerType);

private:
    void maybeEmitTriggered();
    void emitTriggered();

    QTimer m_timer;
    bool m_hasPendingEmission;
};

// Convenience subclasses, e.g. for registering into QML

class QSignalLeadingThrottler : public QGenericSignalThrottler
{
    Q_OBJECT
    QML_NAMED_ELEMENT(SignalLeadingThrottler)

public:
    explicit QSignalLeadingThrottler(QObject *parent = nullptr) :
        QGenericSignalThrottler(Type::Throttler, EmissionPolicy::Leading, parent)
    {}
};

class QSignalTrailingThrottler : public QGenericSignalThrottler
{
    Q_OBJECT
    QML_NAMED_ELEMENT(SignalTrailingThrottler)

public:
    explicit QSignalTrailingThrottler(QObject *parent = nullptr) :
        QGenericSignalThrottler(Type::Throttler, EmissionPolicy::Trailing, parent)
    {}
};

class QSignalLeadingDebouncer : public QGenericSignalThrottler
{
    Q_OBJECT
    QML_NAMED_ELEMENT(SignalLeadingDebouncer)

public:
    explicit QSignalLeadingDebouncer(QObject *parent = nullptr) :
        QGenericSignalThrottler(Type::Debouncer, EmissionPolicy::Leading, parent)
    {}
};

class QSignalTrailingDebouncer : public QGenericSignalThrottler
{
    Q_OBJECT
    QML_NAMED_ELEMENT(SignalTrailingDebouncer)

public:
    explicit QSignalTrailingDebouncer(QObject *parent = nullptr) :
        QGenericSignalThrottler(Type::Debouncer, EmissionPolicy::Trailing, parent)
    {}
};

#endif // QSIGNALTHROTTLER_H
