#include "networksettingsinterface_p.h"
#include "connman_technology_interface.h"
#include "connmancommon.h"
#include "solid_log.h"

NetworkSettingsInterfacePrivate::NetworkSettingsInterfacePrivate(NetworkSettingsInterface* parent)
    : QObject(parent)
    , m_technology(nullptr)
    ,q_ptr(parent)
{
}

void NetworkSettingsInterfacePrivate::initialize(const QString& path, const QVariantMap& properties)
{
    m_technology = new NetConnmanTechnologyInterface(QStringLiteral("net.connman"), path,
                                                  QDBusConnection::systemBus(), this);
    connect(m_technology, SIGNAL(PropertyChanged(QString,QDBusVariant)),
           this, SLOT(updateProperty(QString,QDBusVariant)));

    updateProperty(Q_PropertyName, properties[Q_PropertyName]);
    updateProperty(Q_PropertyType, properties[Q_PropertyType]);
    updateProperty(Q_PropertyConnected, properties[Q_PropertyConnected]);
    updateProperty(Q_PropertyPowered, properties[Q_PropertyPowered]);
}

void NetworkSettingsInterfacePrivate::updateProperty(const QString &name, const QDBusVariant &value)
{
    updateProperty(name, value.variant());
}

void NetworkSettingsInterfacePrivate::updateProperty(const QString &name, const QVariant &value)
{
    Q_Q(NetworkSettingsInterface);
    if (name == Q_PropertyName) {
        m_name = qdbus_cast<QString>(value);
    }
    else if (name == Q_PropertyType) {
        qdbus_cast<QString>(value) >> m_type;
        emit q->typeChanged();
    }
    else if (name == Q_PropertyConnected) {
        bool connected = qdbus_cast<bool>(value);
        if (connected)
            m_state.setState(NetworkSettingsState::Online);
        else
            m_state.setState(NetworkSettingsState::Disconnect);
        emit q->stateChanged();
    }
    else if (name == Q_PropertyPowered) {
        m_powered = qdbus_cast<bool>(value);
        emit q->poweredChanged();
    }
}

void NetworkSettingsInterfacePrivate::setState(NetworkSettingsState::State aState)
{
    Q_Q(NetworkSettingsInterface);
    m_state.setState(aState);
    emit q->stateChanged();
}

void NetworkSettingsInterfacePrivate::setPowered(const bool aPowered)
{
    m_technology->SetProperty(Q_PropertyPowered, QDBusVariant(QVariant(aPowered)));
}

void NetworkSettingsInterfacePrivate::scan()
{
    if (m_powered)
        m_technology->Scan();
    else
        SOLIDLOG_TRACE()<<"Tried to scan while not powered"<<type();
}

QString NetworkSettingsInterfacePrivate::path() const
{
    return m_technology->path();
}
