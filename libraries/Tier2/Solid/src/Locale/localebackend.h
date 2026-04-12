#ifndef LOCALEBACKEND_H
#define LOCALEBACKEND_H

#include <QDefs>

class LocaleBackend: public QObject
{
public:
    enum Capabilities
    {
        Locale = ( 1<<0 ),
    };

    explicit LocaleBackend(QObject* parent=nullptr): QObject(parent) {};

    bool hasCapability(int capability) { return getCapabilities() & capability; };
    bool canSetLocale() { return hasCapability(Capabilities::Locale); }

    virtual int getCapabilities() = 0;

    virtual QString getLocale() const { return QLocale::system().name(); };
    virtual bool setLocale(const QString& locale) { Q_UNUSED(locale); return false; };
};

#endif // LOCALEBACKEND_H
