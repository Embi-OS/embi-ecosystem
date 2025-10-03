#ifndef UPTIMEINFO_H
#define UPTIMEINFO_H

#include <QDefs>

class UptimeInfo : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(double systemTime READ systemTime NOTIFY updated FINAL)
    Q_PROPERTY(double idleTime READ idleTime NOTIFY updated FINAL)
    Q_PROPERTY(double upSeconds READ upSeconds NOTIFY updated FINAL)
    Q_PROPERTY(double upMinutes READ upMinutes NOTIFY updated FINAL)
    Q_PROPERTY(double upHours READ upHours NOTIFY updated FINAL)
    Q_PROPERTY(double upDays READ upDays NOTIFY updated FINAL)
    Q_PROPERTY(QString upTime READ upTime NOTIFY updated FINAL)

    Q_WRITABLE_VAR_PROPERTY(int, tick, Tick, 0)

public:
    explicit UptimeInfo(QObject *parent = nullptr);

    double systemTime() const;
    double idleTime() const;
    int upSeconds() const;
    int upMinutes() const;
    int upHours() const;
    int upDays() const;
    const QString& upTime() const;

public slots:
    bool refresh();

signals:
    void updated();

private:
    double m_systemTime = 0;
    double m_idleTime = 0;
    int m_upSeconds = 0;
    int m_upMinutes = 0;
    int m_upHours = 0;
    int m_upDays = 0;
    QString m_upTime = 0;
};

#endif // UPTIMEINFO_H
