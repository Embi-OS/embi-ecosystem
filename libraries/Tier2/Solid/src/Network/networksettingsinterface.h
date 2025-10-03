#ifndef NETWORKSETTINGSINTERFACE_H
#define NETWORKSETTINGSINTERFACE_H

#include <QDefs>
#include "networksettings.h"

class NetworkSettingsInterfacePrivate;
class NetworkSettingsInterface : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(NetworkSettingsState::State state READ state NOTIFY stateChanged FINAL)
    Q_PROPERTY(NetworkSettingsType::Type type READ type NOTIFY typeChanged FINAL)
    Q_PROPERTY(bool powered READ powered WRITE setPowered NOTIFY poweredChanged FINAL)

public:
    explicit NetworkSettingsInterface(QObject* parent = nullptr);
    QString name() const;
    NetworkSettingsState::State state();
    NetworkSettingsType::Type type();
    bool powered() const;
    void setPowered(const bool powered);
    Q_INVOKABLE void scanServices();

Q_SIGNALS:
    void nameChanged();
    void stateChanged();
    void typeChanged();
    void poweredChanged();

protected:
    friend class NetworksInterfaceModel;
    NetworkSettingsInterfacePrivate *d_ptr;

    Q_DISABLE_COPY(NetworkSettingsInterface)
    Q_DECLARE_PRIVATE(NetworkSettingsInterface)
};

#endif // NETWORKSETTINGSINTERFACE_H
