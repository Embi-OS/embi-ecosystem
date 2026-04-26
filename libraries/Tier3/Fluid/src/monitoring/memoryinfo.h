#ifndef MEMORYINFO_H
#define MEMORYINFO_H

#include <QDefs>

class MemoryInfo : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool isNull READ isNull NOTIFY updated FINAL)
    Q_PROPERTY(quint64 totalPhysical READ totalPhysical NOTIFY updated FINAL)
    Q_PROPERTY(quint64 freePhysical READ freePhysical NOTIFY updated FINAL)
    Q_PROPERTY(quint64 availablePhysical READ availablePhysical NOTIFY updated FINAL)
    Q_PROPERTY(quint64 cached READ cached NOTIFY updated FINAL)
    Q_PROPERTY(quint64 buffers READ buffers NOTIFY updated FINAL)
    Q_PROPERTY(quint64 totalSwapFile READ totalSwapFile NOTIFY updated FINAL)
    Q_PROPERTY(quint64 freeSwapFile READ freeSwapFile NOTIFY updated FINAL)
    Q_PROPERTY(float usage READ usage NOTIFY updated FINAL)

    Q_WRITABLE_VAR_PROPERTY(int, tick, Tick, 0)

public:
    explicit MemoryInfo(QObject *parent = nullptr);

    bool isNull() const;

    quint64 totalPhysical() const;
    quint64 freePhysical() const;
    quint64 availablePhysical() const;
    quint64 cached() const;
    quint64 buffers() const;
    quint64 totalSwapFile() const;
    quint64 freeSwapFile() const;
    float usage() const;

public slots:
    bool refresh();

signals:
    void updated();

private:
    quint64 m_totalPhysical = 0;
    quint64 m_availablePhysical = 0;
    quint64 m_freePhysical = 0;
    quint64 m_totalSwapFile = 0;
    quint64 m_freeSwapFile = 0;
    quint64 m_cached = 0;
    quint64 m_buffers = 0;
};

#endif // MEMORYINFO_H
