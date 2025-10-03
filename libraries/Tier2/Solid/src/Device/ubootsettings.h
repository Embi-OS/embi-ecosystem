#ifndef UBOOTSETTINGS_H
#define UBOOTSETTINGS_H

#include <QDefs>
#include <QModels>

#include <QFutureWatcher>

class UBootSettings : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool canPrintEnv    READ canPrintEnv    NOTIFY capabilitiesChanged FINAL)
    Q_PROPERTY(bool canSetEnv      READ canSetEnv      NOTIFY capabilitiesChanged FINAL)
    Q_PROPERTY(bool canSetOverlays READ canSetOverlays NOTIFY capabilitiesChanged FINAL)

public:
    explicit UBootSettings(QObject *parent = nullptr);

    static bool canPrintEnv();
    static bool canSetEnv();
    static bool canSetOverlays();

    Q_INVOKABLE static QString printEnv(const QString& name);
    Q_INVOKABLE static bool setEnv(const QString& name, const QString& value);
    Q_INVOKABLE static bool clearEnv(const QString& name);

    Q_INVOKABLE static QStringList readOverlays();
    Q_INVOKABLE static bool writeOverlays(const QStringList& overlays);

signals:
    void capabilitiesChanged();
};

class UBootEnvModel: public QVariantListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit UBootEnvModel(QObject* parent=nullptr);

protected:
    bool doSelect() override final;

    QPointer<QFutureWatcher<QVariantList>> m_selectWatcher;
};

#endif // UBOOTSETTINGS_H
