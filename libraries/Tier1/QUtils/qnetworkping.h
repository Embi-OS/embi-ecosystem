#ifndef QNETWORKPING_H
#define QNETWORKPING_H

#include <QDefs>
#include <QProcess>

class QNetworkPing : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(NetworkPing)

    Q_WRITABLE_REF_PROPERTY(QString, address, Address, {})
    Q_WRITABLE_VAR_PROPERTY(int, timeout, Timeout, 30000)
    Q_WRITABLE_VAR_PROPERTY(int, interval, Interval, 1000)

public:
    explicit QNetworkPing(QObject *parent = nullptr);

    static double ping(const QString &address, int timeout=30000, int interval=1000);
    static void ping(const QString &address, const QObject* context, std::function<void(double)> callback, int timeout=30000, int interval=1000);

public slots:
    void ping();

signals:
    void finished(double result);
};

#endif // QNETWORKPING_H
