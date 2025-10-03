#ifndef CPUSTATMODEL_H
#define CPUSTATMODEL_H

#include <QDefs>
#include <QModels>

struct CpuStat {
    Q_GADGET
    QML_VALUE_TYPE(cpuStat)

    Q_MEMBER_PROPERTY(QString, name, "")
    Q_MEMBER_PROPERTY(quint64, user, 0)
    Q_MEMBER_PROPERTY(quint64, nice, 0)
    Q_MEMBER_PROPERTY(quint64, system, 0)
    Q_MEMBER_PROPERTY(quint64, idle, 0)
    Q_MEMBER_PROPERTY(quint64, iowait, 0)
    Q_MEMBER_PROPERTY(quint64, irq, 0)
    Q_MEMBER_PROPERTY(quint64, softirq, 0)
    Q_MEMBER_PROPERTY(quint64, steal, 0)
    Q_MEMBER_PROPERTY(quint64, guest, 0)
    Q_MEMBER_PROPERTY(quint64, guestNice, 0)

    Q_MEMBER_PROPERTY(quint64, total, 0)
    Q_MEMBER_PROPERTY(quint64, userTotal, 0)
    Q_MEMBER_PROPERTY(double, usage, 0)

public:
    CpuStat() = default;
    ~CpuStat() = default;
    Q_INVOKABLE CpuStat(const CpuStat &other) = default;
};
Q_DECLARE_METATYPE(CpuStat)

class CpuStatModel: public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int length READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int size READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int isEmpty READ isEmpty NOTIFY emptyChanged FINAL)

    Q_PROPERTY(CpuStat cpu READ cpu NOTIFY updated FINAL)
    Q_PROPERTY(QList<CpuStat> cpus READ cpus NOTIFY updated FINAL)
    Q_PROPERTY(QList<quint64> intr READ intr NOTIFY updated FINAL)
    Q_PROPERTY(quint64 ctxt READ ctxt NOTIFY updated FINAL)
    Q_PROPERTY(quint64 btime READ btime NOTIFY updated FINAL)
    Q_PROPERTY(quint64 processes READ processes NOTIFY updated FINAL)
    Q_PROPERTY(quint64 procsRunning READ procsRunning NOTIFY updated FINAL)
    Q_PROPERTY(quint64 procsBlocked READ procsBlocked NOTIFY updated FINAL)
    Q_PROPERTY(QList<quint64> softirq READ softirq NOTIFY updated FINAL)

    Q_WRITABLE_VAR_PROPERTY(int, tick, Tick, 0)

public:
    explicit CpuStatModel(QObject* parent=nullptr);

    enum {
        CpuRole = Qt::UserRole,
        NameRole,
        UserRole,
        NiceRole,
        SystemRole,
        IdleRole,
        IOWaitRole,
        IrqRole,
        SoftIrqRole,
        StealRole,
        GuestRole,
        GestNiceRole,
        TotalRole,
        UserTotalRole,
        UsageRole,
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const { return rowCount(); };
    int size() const { return rowCount(); };
    int length() const { return rowCount(); };
    bool isEmpty() const { return rowCount() == 0; };

    const CpuStat& cpu() const;
    const QList<CpuStat>& cpus() const;
    const QList<quint64>& intr() const;
    quint64 ctxt() const;
    quint64 btime() const;
    quint64 processes() const;
    quint64 procsRunning() const;
    quint64 procsBlocked() const;
    const QList<quint64>& softirq() const;

public slots:
    bool refresh();

signals:
    void updated();
    void countChanged(int count);
    void emptyChanged(bool empty);

private slots:
    void countInvalidate();

private:
    int m_count=0;
    QHash<int, QByteArray> m_roleNames;

    CpuStat m_cpu;
    QList<CpuStat> m_cpus;
    QList<quint64> m_intr;
    quint64 m_ctxt = 0;
    quint64 m_btime = 0;
    quint64 m_processes = 0;
    quint64 m_procsRunning = 0;
    quint64 m_procsBlocked = 0;
    QList<quint64> m_softirq;
};

#endif // CPUSTATMODEL_H
