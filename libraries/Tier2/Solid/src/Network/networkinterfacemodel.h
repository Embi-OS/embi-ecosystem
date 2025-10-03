#ifndef NETWORKINTERFACEMODEL_H
#define NETWORKINTERFACEMODEL_H

#include <QNetworkInterface>
#include <QAbstractListModel>
#include <QDefs>

class NetworkInterfaceModel: public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int length READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int size READ size NOTIFY countChanged FINAL)
    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY emptyChanged FINAL)

    Q_WRITABLE_VAR_PROPERTY(int, refreshInterval, RefreshInterval, 0)

public:
    explicit NetworkInterfaceModel(QObject *parent = nullptr);

    enum {
        Type = Qt::UserRole + 1,
        Name,
        Identifier,
        Connected,
        Up,
        Running,
        Address,
        MAC
    };

    enum InterfaceType {
        Loopback = QNetworkInterface::Loopback,
        Virtual = QNetworkInterface::Virtual,
        Ethernet = QNetworkInterface::Ethernet,
        Slip = QNetworkInterface::Slip,
        CanBus = QNetworkInterface::CanBus,
        Ppp = QNetworkInterface::Ppp,
        Fddi = QNetworkInterface::Fddi,
        Wifi = QNetworkInterface::Wifi,
        Ieee80211 = QNetworkInterface::Ieee80211,
        Phonet = QNetworkInterface::Phonet,
        Ieee802154 = QNetworkInterface::Ieee802154,
        SixLoWPAN = QNetworkInterface::SixLoWPAN,
        Ieee80216 = QNetworkInterface::Ieee80216,
        Ieee1394 = QNetworkInterface::Ieee1394,

        Unknown = QNetworkInterface::Unknown
    };
    Q_ENUM(InterfaceType)

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const { return rowCount(); };
    int size() const { return rowCount(); };
    int length() const { return rowCount(); };
    bool isEmpty() const { return rowCount() == 0; };

    Q_INVOKABLE bool contains(const QString& name) const;

public slots:
    void refresh();

signals:
    void updated();
    void countChanged(int count);
    void emptyChanged(bool empty);

private slots:
    void countInvalidate();

private:
    int m_count=0;
    QHash<int, QByteArray> m_roleNames;

    QList<QNetworkInterface> m_interfaces;
};

#endif // NETWORKINTERFACEMODEL_H
