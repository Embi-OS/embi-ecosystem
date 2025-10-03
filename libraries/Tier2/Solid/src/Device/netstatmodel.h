#ifndef NETSTATMODEL_H
#define NETSTATMODEL_H

#include <QDefs>
#include <QModels>

struct NetStat {
    Q_GADGET
    QML_VALUE_TYPE(netStat)

    Q_MEMBER_PROPERTY(QString, interface, "")
    Q_MEMBER_PROPERTY(quint64, rxBytes, 0)
    Q_MEMBER_PROPERTY(quint64, rxPackets, 0)
    Q_MEMBER_PROPERTY(quint64, rxErrs, 0)
    Q_MEMBER_PROPERTY(quint64, rxDrop, 0)
    Q_MEMBER_PROPERTY(quint64, rxFifo, 0)
    Q_MEMBER_PROPERTY(quint64, rxFrame, 0)
    Q_MEMBER_PROPERTY(quint64, rxCompressed, 0)
    Q_MEMBER_PROPERTY(quint64, rxMulticast, 0)
    Q_MEMBER_PROPERTY(double, rxThroughput, 0)
    Q_MEMBER_PROPERTY(quint64, txBytes, 0)
    Q_MEMBER_PROPERTY(quint64, txPackets, 0)
    Q_MEMBER_PROPERTY(quint64, txErrs, 0)
    Q_MEMBER_PROPERTY(quint64, txDrop, 0)
    Q_MEMBER_PROPERTY(quint64, txFifo, 0)
    Q_MEMBER_PROPERTY(quint64, txColls, 0)
    Q_MEMBER_PROPERTY(quint64, txCarrier, 0)
    Q_MEMBER_PROPERTY(quint64, txCompressed, 0)
    Q_MEMBER_PROPERTY(double, txThroughput, 0)

public:
    NetStat() = default;
    ~NetStat() = default;
    Q_INVOKABLE NetStat(const NetStat &other) = default;
};
Q_DECLARE_METATYPE(NetStat)

class NetStatModel: public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int length READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int size READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int isEmpty READ isEmpty NOTIFY emptyChanged FINAL)

    Q_PROPERTY(NetStat net READ net NOTIFY updated FINAL)
    Q_PROPERTY(QList<NetStat> nets READ nets NOTIFY updated FINAL)

    Q_WRITABLE_VAR_PROPERTY(int, tick, Tick, 0)

public:
    explicit NetStatModel(QObject* parent=nullptr);

    enum {
        NetRole = Qt::UserRole,
        InterfaceRole,
        RxBytesRole,
        RxPacketsRole,
        RxErrsRole,
        RxDropRole,
        RxFifoRole,
        RxFrameRole,
        RxCompressedRole,
        RxMulticastRole,
        RxThroughputRole,
        TxBytesRole,
        TxPacketsRole,
        TxErrsRole,
        TxDropRole,
        TxFifoRole,
        TxCollsRole,
        TxCarrierRole,
        TxCompressedRole,
        TxThroughputRole,
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const { return rowCount(); };
    int size() const { return rowCount(); };
    int length() const { return rowCount(); };
    bool isEmpty() const { return rowCount() == 0; };

    const NetStat& net() const;
    const QList<NetStat>& nets() const;

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

    NetStat m_net;
    QList<NetStat> m_nets;

    QElapsedTimer m_time;
};

#endif // NETSTATMODEL_H
