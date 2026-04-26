#ifndef LOCALESETTINGS_H
#define LOCALESETTINGS_H

#include <QDefs>
#include "qsingleton.h"

class OrgFreedesktopLocale1Interface;
class QTranslatorLoader;
class LocaleSettings : public QObject,
                       public QQmlSingleton<LocaleSettings>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool canSetLocale   READ canSetLocale   CONSTANT FINAL)
    Q_PROPERTY(QString locale      READ getLocale      WRITE setLocale NOTIFY localeChanged FINAL)

protected:
    friend QQmlSingleton<LocaleSettings>;
    explicit LocaleSettings(QObject *parent = nullptr);

public:
    static void init();

    enum Capabilities
    {
        Locale = ( 1<<0 ),
    };

    int getCapabilities() const;

    bool hasCapability(int capability) const { return getCapabilities() & capability; };
    bool canSetLocale() const { return hasCapability(Capabilities::Locale); }

    QString getLocale() const;

public slots:
    bool setLocale(const QString& locale);

signals:
    void localeChanged();

private:
    bool reloadTranslator();
    void updateLocale();

    OrgFreedesktopLocale1Interface* m_interface=nullptr;
    QHash<QString, QString> m_localeCache;

    QTranslatorLoader* m_translator=nullptr;
};

#endif // LOCALESETTINGS_H
