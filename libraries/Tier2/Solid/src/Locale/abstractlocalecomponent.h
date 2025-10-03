#ifndef ABSTRACTLOCALECOMPONENT_H
#define ABSTRACTLOCALECOMPONENT_H

#include <QString>

class AbstractLocaleComponent
{
public:
    AbstractLocaleComponent() {};

    enum Capabilities
    {
        Locale = 0x01
    };

    bool hasCapability(int capability) { return getCapabilities() & capability; };
    bool canSetLocale() { return hasCapability(Capabilities::Locale); }

    virtual int getCapabilities() = 0;

    virtual QString getLocale() const { return QString(); };
    virtual bool setLocale(const QString& locale) { Q_UNUSED(locale); return false; };
};

#endif // ABSTRACTLOCALECOMPONENT_H
