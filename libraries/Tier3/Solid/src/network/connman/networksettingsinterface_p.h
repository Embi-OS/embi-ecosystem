#ifndef NETWORKSETTINGSINTERFACEPRIVATE_H
#define NETWORKSETTINGSINTERFACEPRIVATE_H

#include <QObject>
#include <QDBusVariant>
#include "networksettings.h"
#include "networksettingsinterface.h"

// Automatically generated class in global namespace
class NetConnmanTechnologyInterface;

class NetworkSettingsInterfacePrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(NetworkSettingsInterface)
public:
    explicit NetworkSettingsInterfacePrivate(NetworkSettingsInterface* parent);
    void initialize(const QString& path, const QVariantMap& properties);
    void setPowered(const bool power);
    void setState(NetworkSettingsState::State aState);
    void scan();
    QString name() const {return m_name;}
    NetworkSettingsType::Type type() const {return m_type.type();}
    NetworkSettingsState::State state() const {return m_state.state();}
    bool powered() const {return m_powered;}
    QString path() const;

public slots:
    void updateProperty(const QString &name, const QDBusVariant &value);
protected:
    void updateProperty(const QString &name, const QVariant &value);

protected:
    NetConnmanTechnologyInterface *m_technology;
    QString m_name;
    NetworkSettingsType m_type;
    NetworkSettingsState m_state;
    bool m_powered;
    NetworkSettingsInterface *q_ptr;
};


class ConnmanSettingsInterface : public NetworkSettingsInterface
{
    Q_OBJECT
public:
    ConnmanSettingsInterface(const QString& path, const QVariantMap& properties, QObject *parent = nullptr)
        :NetworkSettingsInterface(parent)
    {
        if (d_ptr)
            d_ptr->initialize(path, properties);
    }

    void setState(NetworkSettingsState::State aState) {
        Q_D(NetworkSettingsInterface);
        d->setState(aState);
    }

    virtual ~ConnmanSettingsInterface() {}

    QString path() const {
        if (d_ptr)
            return d_ptr->path();
        return QString();
    }
};

#endif // NETWORKSETTINGSINTERFACEPRIVATE_H
