#ifndef LANGUAGESETTINGS_H
#define LANGUAGESETTINGS_H

#include <QDefs>
#include "qsingleton.h"

class QTranslatorLoader;
class LanguageSettings : public QObject,
                         public QQmlSingleton<LanguageSettings>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged FINAL)
    Q_PROPERTY(QString effectiveLanguage READ effectiveLanguage NOTIFY effectiveLanguageChanged FINAL)

protected:
    friend QQmlSingleton<LanguageSettings>;
    explicit LanguageSettings(QObject *parent = nullptr);

public:
    static void init(QQmlEngine* engine = nullptr);
    static QString systemLanguageName();

    const QString& language() const;
    const QString& effectiveLanguage() const;

    void setEngine(QQmlEngine* engine);

public slots:
    bool setLanguage(const QString& language);
    bool setLanguage(const QString& language, bool persist);

    bool resetToSystemLanguage();
    void refreshSystemLanguage();
    bool reloadTranslations();

signals:
    void languageChanged(const QString& language);
    void effectiveLanguageChanged(const QString& effectiveLanguage);

private:
    QString systemLanguage() const;
    QString normalizedLanguage(const QString& language) const;
    bool isSystemLanguage() const;
    void readSettings();
    void writeSettings();
    bool updateEffectiveLanguage();

    QTranslatorLoader* m_translator=nullptr;
    QString m_language=systemLanguageName();
    QString m_effectiveLanguage;
};

#endif // LANGUAGESETTINGS_H
