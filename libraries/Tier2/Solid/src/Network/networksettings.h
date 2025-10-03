#ifndef NETWORKSETTINGS_H
#define NETWORKSETTINGS_H

#include "networksettingsaddressmodel.h"

#include <QDefs>
#include <QUrl>
#include <QNetworkInterface>

class NetworkSettingsState : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(State state READ state WRITE setState NOTIFY stateChanged FINAL)

public:
    enum State {
        Idle = 0,
        Failure,
        Association,
        Configuration,
        Ready,
        Disconnect,
        Online,
        Undefined
    };
    Q_ENUM(State)

    explicit NetworkSettingsState(State state, QObject *parent = nullptr);
    explicit NetworkSettingsState(QObject *parent = nullptr);

    State state() const;
    void setState(const State state);

Q_SIGNALS:
    void stateChanged();

private:
    State m_state;
};

class NetworkSettingsType : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged FINAL)

public:
    enum Type {
        Wired = 0,
        Wifi,
        Bluetooth,
        Unknown
    };
    Q_ENUM(Type)

    explicit NetworkSettingsType(Type type, QObject *parent = nullptr);
    explicit NetworkSettingsType(QObject *parent = nullptr);

    Type type() const;
    void setType(const Type type);

Q_SIGNALS:
    void typeChanged();

private:
    Type m_type;
};

class NetworkSettingsIPv4 : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
    explicit NetworkSettingsIPv4(QObject *parent = nullptr);

    enum Method {
        Dhcp = 0,
        Manual,
        Off
    };
    Q_ENUM(Method)

    Q_WRITABLE_REF_PROPERTY(QString, address, Address, "")
    Q_WRITABLE_REF_PROPERTY(QString, gateway, Gateway, "")
    Q_WRITABLE_VAR_PROPERTY(NetworkSettingsIPv4::Method, method, Method, Dhcp)
    Q_WRITABLE_REF_PROPERTY(QString, mask, Mask, "")
};

class NetworkSettingsIPv6 : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
    explicit NetworkSettingsIPv6(QObject *parent = nullptr);

    enum Method {
        Auto = 0,
        Manual,
        Off
    };
    Q_ENUM(Method)

    enum Privacy {
        Disabled = 0,
        Enabled,
        Preferred
    };
    Q_ENUM(Privacy)

    Q_WRITABLE_REF_PROPERTY(QString, address, Address, "")
    Q_WRITABLE_REF_PROPERTY(QString, gateway, Gateway, "")
    Q_WRITABLE_VAR_PROPERTY(NetworkSettingsIPv6::Method, method, Method, Auto)
    Q_WRITABLE_VAR_PROPERTY(NetworkSettingsIPv6::Privacy, privacy, Privacy, Disabled)
    Q_WRITABLE_VAR_PROPERTY(int, prefixLength, PrefixLength, 0)
};

class NetworkSettingsProxy : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged FINAL)
    Q_PROPERTY(QAbstractItemModel* servers READ servers NOTIFY serversChanged FINAL)
    Q_PROPERTY(QAbstractItemModel* excludes READ excludes NOTIFY excludesChanged FINAL)
    Q_PROPERTY(Method method READ method WRITE setMethod NOTIFY methodChanged FINAL)
public:
    explicit NetworkSettingsProxy(QObject *parent = nullptr);

    enum Method {
        Direct = 0,
        Auto,
        Manual
    };
    Q_ENUM(Method)

    QUrl url() const;
    void setUrl(const QUrl& url);
    QAbstractItemModel* servers();
    void setServers(const QStringList& servers);
    QStringList servers() const;
    QAbstractItemModel* excludes();
    QStringList excludes() const;
    void setExcludes(const QStringList& excludes);
    Method method() const;
    void setMethod(const Method& method);

Q_SIGNALS:
    void urlChanged();
    void serversChanged();
    void excludesChanged();
    void methodChanged();
private:
    QUrl m_url;
    NetworkSettingsAddressModel m_servers;
    NetworkSettingsAddressModel m_excludes;
    Method m_method;
};

class NetworkSettingsWireless : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(int signalStrength READ signalStrength WRITE setSignalStrength NOTIFY signalStrengthChanged FINAL)
    Q_PROPERTY(bool hidden READ hidden NOTIFY hiddenChanged FINAL)
    Q_PROPERTY(bool isOutOfRange READ outOfRange WRITE setOutOfRange NOTIFY outOfRangeChanged FINAL)
public:
    explicit NetworkSettingsWireless(QObject* parent = nullptr);

    enum class Security {
        None    = 0x0000,
        WEP     = 0x0002,
        WPA     = 0x0004,
        WPA2    = 0x0008
    };
    Q_DECLARE_FLAGS(Securities, Security)

    Q_INVOKABLE bool supportsSecurity(Security security);
    bool hidden() const;
    void setHidden(const bool hidden);
    int signalStrength() const;
    void setSignalStrength(const int signalStrength);
    void setSecurity(const Securities securities);
    void setOutOfRange(const bool aOutOfRange);
    bool outOfRange() const;

Q_SIGNALS:
    void hiddenChanged();
    void signalStrengthChanged();
    void passwordChanged();
    void outOfRangeChanged();
private:
    Securities m_supportedSecurites;
    bool m_hidden;
    int m_signalStrength;
    bool m_isOutOfRange;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(NetworkSettingsWireless::Securities)


class NetworkSettingsEthernet : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
    explicit NetworkSettingsEthernet(QObject* parent = nullptr);

    enum Method {
        Auto = 0,
        Manual,
        Off
    };
    Q_ENUM(Method)

    Q_READONLY_VAR_PROPERTY(NetworkSettingsEthernet::Method, method, Method, Auto)
    Q_READONLY_REF_PROPERTY(QString, address, Address, "")
    Q_READONLY_REF_PROPERTY(QString, interface, Interface, "")
};

#endif //NETWORKSETTINGS_H
