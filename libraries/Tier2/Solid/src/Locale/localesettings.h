#ifndef LOCALESETTINGS_H
#define LOCALESETTINGS_H

#include <QDefs>
#include "qsingleton.h"

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

    static bool canSetLocale();

    QString getLocale() const;

public slots:
    bool setLocale(const QString& locale);

signals:
    void localeChanged();

private:
    bool reloadTranslator();
    QTranslatorLoader* m_translator=nullptr;
};

#endif // LOCALESETTINGS_H
