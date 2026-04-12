#ifndef LOCALEBACKENDDBUS_H
#define LOCALEBACKENDDBUS_H

#include "localebackend.h"

class OrgFreedesktopLocale1Interface;
class LocaleBackendDbus final : public LocaleBackend
{
public:
    explicit LocaleBackendDbus(QObject* parent=nullptr);

    int getCapabilities() override;

    QString getLocale() const override;
    bool setLocale(const QString& locale) override;

private:
    void updateLocale();

    OrgFreedesktopLocale1Interface *m_interface;
    QHash<QString, QString> m_localeCache;
};

#endif // LOCALEBACKENDDBUS_H
