#ifndef QWATCHDOG_H
#define QWATCHDOG_H

#include <QObject>
#include <QQmlEngine>
#include <QTimer>
#include <QQmlParserStatus>
#include <QDefs>

class QWatchdogWorker;
class QWatchdog : public QObject,
                  public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Watchdog)
    Q_INTERFACES(QQmlParserStatus)

    Q_WRITABLE_VAR_PROPERTY(bool, enabled, Enabled, true)

public:
    explicit QWatchdog(QObject *parent = nullptr);
    ~QWatchdog();

    void classBegin() override;
    void componentComplete() override;

private slots:
    void onEnabledChanged();
    void onUiBeat();

private:
    void restart(int frequency_ms);
    void start(int frequency_ms);
    void stop();

    bool m_completed = false;
    QTimer *const m_uiTimer;
    QThread *m_watchDogThread = nullptr;
    QWatchdogWorker *m_worker = nullptr;
};

#endif // QWATCHDOG_H
