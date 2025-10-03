#ifndef QSCANEVENTNOTIFIER_H
#define QSCANEVENTNOTIFIER_H

#include <QQmlEngine>
#include <QObject>
#include <QQmlParserStatus>
#include <QDefs>

class QScanEventNotifier : public QObject,
                           public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ScanEventNotifier)
    Q_INTERFACES(QQmlParserStatus)

    Q_WRITABLE_VAR_PROPERTY(bool, enabled, Enabled, true)
    Q_WRITABLE_VAR_PROPERTY(int, elapsedThreshold, ElapsedThreshold, 100)

    Q_READONLY_REF_PROPERTY(QString, lastValue, LastValue, {})

public:
    explicit QScanEventNotifier(QObject *parent = nullptr);

    void classBegin() override;
    void componentComplete() override;

public slots:
    void clearLastValue();

signals:
    void newScan(const QString &scanValue);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QString m_scan = "";
    quint64 m_timestamp = 0;
};

#endif // QSCANEVENTNOTIFIER_H
